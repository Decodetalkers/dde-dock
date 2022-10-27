/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     chenwei <chenwei@uniontech.com>
 *
 * Maintainer: chenwei <chenwei@uniontech.com>
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bluetoothapplet.h"
#include "device.h"
#include "bluetoothconstants.h"
#include "adaptersmanager.h"
#include "adapter.h"
#include "bluetoothadapteritem.h"
#include "horizontalseperator.h"

#include <DApplicationHelper>
#include <DDBusSender>
#include <DLabel>
#include <DSwitchButton>
#include <DScrollArea>
#include <DListView>

#include <QString>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QDebug>
#include <QScroller>
#include <QMouseEvent>

SettingLabel::SettingLabel(QString text, QWidget *parent)
    : QWidget(parent)
    , m_label(new DLabel(text, this))
    , m_layout(new QHBoxLayout(this))
{
    setAccessibleName("BluetoothSettingLabel");
    setContentsMargins(0, 0, 0, 0);
    m_layout->setMargin(0);
    m_layout->setSpacing(4);
    m_layout->setContentsMargins(20, 0, 6, 0);
    m_layout->addWidget(m_label, 0, Qt::AlignLeft | Qt::AlignHCenter);
    m_layout->addStretch();

    setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Background, Qt::transparent);
    this->setPalette(p);

    m_label->setForegroundRole(QPalette::BrightText);
    updateEnabledStatus();
}

void SettingLabel::addButton(QWidget *button, int space)
{
    m_layout->addWidget(button, 0, Qt::AlignRight | Qt::AlignHCenter);
    m_layout->addSpacing(space);
}

void SettingLabel::updateEnabledStatus()
{
    QPalette p = m_label->palette();
    if (m_label->isEnabled())
        p.setColor(QPalette::BrightText, QColor(0, 0, 0));
    else
        p.setColor(QPalette::BrightText, QColor(51, 51, 51));
    m_label->setPalette(p);
}

void SettingLabel::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange)
        updateEnabledStatus();

    QWidget::changeEvent(event);
}

void SettingLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        Q_EMIT clicked();
        return;
    }

    return QWidget::mousePressEvent(ev);
}

void SettingLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    if (DApplicationHelper::instance()->themeType() == DApplicationHelper::LightType) {
        painter.setBrush(QColor(0, 0, 0, 0.03 * 255));
    } else {
        painter.setBrush(QColor(255, 255, 255, 0.03 * 255));
    }
    painter.drawRoundedRect(rect(), 0, 0);

    return QWidget::paintEvent(event);
}

BluetoothApplet::BluetoothApplet(AdaptersManager *adapterManager, QWidget *parent)
    : QWidget(parent)
    , m_scroarea(nullptr)
    , m_contentWidget(new QWidget(this))
    , m_adaptersManager(adapterManager)
    , m_settingLabel(new SettingLabel(tr("Bluetooth settings"), this))
    , m_mainLayout(new QVBoxLayout(this))
    , m_contentLayout(new QVBoxLayout(m_contentWidget))
    , m_seperator(new HorizontalSeperator(this))
    , m_airPlaneModeInter(new DBusAirplaneMode("org.deepin.daemon.AirplaneMode1", "/org/deepin/daemon/AirplaneMode1", QDBusConnection::systemBus(), this))
    , m_airplaneModeEnable(false)
{
    initUi();
    initConnect();

    QScroller::grabGesture(m_scroarea, QScroller::LeftMouseButtonGesture);
    QScrollerProperties propertiesOne = QScroller::scroller(m_scroarea)->scrollerProperties();
    QVariant overshootPolicyOne = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
    propertiesOne.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicyOne);
    QScroller::scroller(m_scroarea)->setScrollerProperties(propertiesOne);
}

bool BluetoothApplet::poweredInitState()
{
    foreach (const auto adapter, m_adapterItems) {
        if (adapter->adapter()->powered()) {
            return true;
        }
    }

    return false;
}

bool BluetoothApplet::hasAadapter()
{
    return m_adaptersManager->adaptersCount();
}

void BluetoothApplet::setAdapterRefresh()
{
    for (BluetoothAdapterItem *adapterItem : m_adapterItems) {
        if (adapterItem->adapter()->discover())
            m_adaptersManager->adapterRefresh(adapterItem->adapter());
    }
    updateSize();
}

void BluetoothApplet::setAdapterPowered(bool state)
{
    for (BluetoothAdapterItem *adapterItem : m_adapterItems) {
        if (adapterItem)
            m_adaptersManager->setAdapterPowered(adapterItem->adapter(), state);
    }
}

QStringList BluetoothApplet::connectedDevicesName()
{
    QStringList deviceList;
    for (BluetoothAdapterItem *adapterItem : m_adapterItems) {
        if (adapterItem)
            deviceList << adapterItem->connectedDevicesName();
    }

    return deviceList;
}

AdaptersManager *BluetoothApplet::adaptersManager()
{
    return m_adaptersManager;
}

void BluetoothApplet::onAdapterAdded(Adapter *adapter)
{
    if (!m_adapterItems.size()) {
        emit justHasAdapter();
    }
    if (m_adapterItems.contains(adapter->id())) {
        onAdapterRemoved(m_adapterItems.value(adapter->id())->adapter());
    }

    BluetoothAdapterItem *adapterItem = new BluetoothAdapterItem(adapter, this);
    connect(adapterItem, &BluetoothAdapterItem::requestSetAdapterPower, this, &BluetoothApplet::onSetAdapterPower);
    connect(adapterItem, &BluetoothAdapterItem::connectDevice, m_adaptersManager, &AdaptersManager::connectDevice);
    connect(adapterItem, &BluetoothAdapterItem::deviceCountChanged, this, &BluetoothApplet::updateSize);
    connect(adapterItem, &BluetoothAdapterItem::adapterPowerChanged, this, &BluetoothApplet::updateBluetoothPowerState);
    connect(adapterItem, &BluetoothAdapterItem::deviceStateChanged, this, &BluetoothApplet::deviceStateChanged);
    connect(adapterItem, &BluetoothAdapterItem::requestRefreshAdapter, m_adaptersManager, &AdaptersManager::adapterRefresh);

    m_adapterItems.insert(adapter->id(), adapterItem);

    m_contentLayout->insertWidget(0, adapterItem, Qt::AlignTop | Qt::AlignVCenter);
    updateBluetoothPowerState();
    updateSize();
}

void BluetoothApplet::onAdapterRemoved(Adapter *adapter)
{
    m_contentLayout->removeWidget(m_adapterItems.value(adapter->id()));
    m_adapterItems.value(adapter->id())->deleteLater();
    m_adapterItems.remove(adapter->id());
    if (m_adapterItems.isEmpty()) {
        emit noAdapter();
    }
    updateBluetoothPowerState();
    updateSize();
}

void BluetoothApplet::onSetAdapterPower(Adapter *adapter, bool state)
{
    m_adaptersManager->setAdapterPowered(adapter, state);
    updateSize();
}

void BluetoothApplet::updateBluetoothPowerState()
{
    foreach (const auto item, m_adapterItems) {
        if (item->adapter()->powered()) {
            emit powerChanged(true);
            return;
        }
    }
    emit powerChanged(false);
    updateSize();
}

void BluetoothApplet::initUi()
{
    setFixedWidth(ItemWidth);
    setAccessibleName("BluetoothApplet");
    setContentsMargins(0, 0, 0, 0);

    m_settingLabel->setFixedHeight(DeviceItemHeight);
    DFontSizeManager::instance()->bind(m_settingLabel->label(), DFontSizeManager::T7);

    m_contentLayout->setMargin(0);
    m_contentLayout->setSpacing(0);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->addWidget(m_seperator);
    m_contentLayout->addWidget(m_settingLabel, 0, Qt::AlignBottom | Qt::AlignVCenter);

    m_scroarea = new QScrollArea(this);

    m_scroarea->setWidgetResizable(true);
    m_scroarea->setFrameStyle(QFrame::NoFrame);
    m_scroarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroarea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_scroarea->setContentsMargins(0, 0, 0, 0);
    m_scroarea->setWidget(m_contentWidget);

    updateIconTheme();

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addWidget(m_scroarea);
    updateSize();

    setAirplaneModeEnabled(m_airPlaneModeInter->enabled());
    setDisabled(m_airPlaneModeInter->enabled());
}

void BluetoothApplet::initConnect()
{
    connect(m_adaptersManager, &AdaptersManager::adapterIncreased, this, &BluetoothApplet::onAdapterAdded);
    connect(m_adaptersManager, &AdaptersManager::adapterDecreased, this, &BluetoothApplet::onAdapterRemoved);
    connect(m_settingLabel, &SettingLabel::clicked, this, [ = ] {
#ifdef USE_AM
        DDBusSender()
        .service("org.deepin.dde.ControlCenter1")
        .interface("org.deepin.dde.ControlCenter1")
        .path("/org/deepin/dde/ControlCenter1")
        .method(QString("ShowPage"))
        .arg(QString("bluetooth"))
        .call();
#else
        DDBusSender()
        .service("com.deepin.dde.ControlCenter")
        .interface("com.deepin.dde.ControlCenter")
        .path("/com/deepin/dde/ControlCenter")
        .method(QString("ShowPage"))
        .arg(QString("bluetooth"))
        .call();
#endif
    });
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this, &BluetoothApplet::updateIconTheme);
    connect(m_airPlaneModeInter, &DBusAirplaneMode::EnabledChanged, this, &BluetoothApplet::setAirplaneModeEnabled);
    connect(m_airPlaneModeInter, &DBusAirplaneMode::EnabledChanged, this, &BluetoothApplet::setDisabled);
}

/**
 * @brief BluetoothApplet::updateIconTheme 根据主题颜色设置蓝牙界面控件背景色
 */
void BluetoothApplet::updateIconTheme()
{
    QPalette widgetBackgroud;
    QPalette scroareaBackgroud;
    if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
        widgetBackgroud.setColor(QPalette::Background, QColor(255, 255, 255, 0.03 * 255));
    else
        widgetBackgroud.setColor(QPalette::Background, QColor(0, 0, 0, 0.03 * 255));

    m_contentWidget->setAutoFillBackground(true);
    m_contentWidget->setPalette(widgetBackgroud);
    scroareaBackgroud.setColor(QPalette::Background, Qt::transparent);
    m_scroarea->setAutoFillBackground(true);
    m_scroarea->setPalette(scroareaBackgroud);
}

void BluetoothApplet::setAirplaneModeEnabled(bool enable)
{
    if (m_airplaneModeEnable == enable)
        return;

    m_airplaneModeEnable = enable;
}

void BluetoothApplet::updateSize()
{
    int height = 0;
    foreach (const auto item, m_adapterItems) {
        height += item->sizeHint().height();
    }

    height += m_seperator->height();

    // 加上蓝牙设置选项的高度
    height += DeviceItemHeight;

    static const int maxHeight = (TitleHeight + TitleSpace) + MaxDeviceCount * DeviceItemHeight;

    setFixedSize(ItemWidth, qMin(maxHeight, height));
}


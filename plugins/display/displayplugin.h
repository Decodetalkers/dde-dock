/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
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

#ifndef DISPLAYPLUGIN_H
#define DISPLAYPLUGIN_H

#include "pluginsiteminterface.h"

#include <QTimer>
#include <QLabel>
#include <QSettings>

namespace Dock{
class TipsWidget;
}

class BrightnessWidget;
class BrightnessModel;
class DisplaySettingWidget;

class DisplayPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "display.json")

public:
    explicit DisplayPlugin(QObject *parent = nullptr);

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface *proxyInter) override;

    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;
    QWidget *itemPopupApplet(const QString &itemKey) override;

    PluginFlags flags() const override;

private:
    QScopedPointer<BrightnessWidget> m_displayWidget;
    QScopedPointer<DisplaySettingWidget> m_displaySettingWidget;
    QScopedPointer<Dock::TipsWidget> m_displayTips;
    QScopedPointer<BrightnessModel> m_model;
};

#endif // DATETIMEPLUGIN_H

/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef SNITRAYWIDGET_H
#define SNITRAYWIDGET_H

#include "constants.h"
#include "basetraywidget.h"
#include "dockpopupwindow.h"

#include "org_kde_statusnotifieritem.h"

#include <QMenu>
#include <QDBusObjectPath>
//DWIDGET_USE_NAMESPACE
//DGUI_USE_NAMESPACE
class DBusMenuImporter;
//namespace Dock {
//class TipsWidget;
//}
//using namespace com::deepin::dde;
using namespace org::kde;

/**
 * @brief The SNITrayWidget class
 * @note 系统托盘第三方程序窗口
 */
class SNITrayItemWidget : public BaseTrayWidget
{
    Q_OBJECT

public:
    enum ItemCategory {UnknownCategory = -1, ApplicationStatus, Communications, SystemServices, Hardware};
    enum ItemStatus {Passive, Active, NeedsAttention};
    enum IconType {UnknownIconType = -1, Icon, OverlayIcon, AttentionIcon, AttentionMovieIcon};

public:
    SNITrayItemWidget(const QString &sniServicePath, QWidget *parent = Q_NULLPTR);

    QString itemKeyForConfig() override;
    void updateIcon() override;
    void sendClick(uint8_t mouseButton, int x, int y) override;

    bool isValid() override;
    SNITrayItemWidget::ItemStatus status();
    SNITrayItemWidget::ItemCategory category();

    static QString toSNIKey(const QString &sniServicePath);
    static bool isSNIKey(const QString &itemKey);
    static QPair<QString, QString> serviceAndPath(const QString &servicePath);
    static uint servicePID(const QString &servicePath);

    void showHoverTips();
    const QPoint topleftPoint() const;
    void showPopupWindow(QWidget *const content, const bool model = false);
    const QPoint popupMarkPoint() const;

    static void setDockPostion(const Dock::Position pos) { DockPosition = pos; }

    QPixmap icon() override;

Q_SIGNALS:
    void statusChanged(SNITrayItemWidget::ItemStatus status);

private Q_SLOTS:
    void initMenu();
    void refreshIcon();
    void refreshOverlayIcon();
    void refreshAttentionIcon();
    void showContextMenu(int x, int y);
    // SNI property change slot
    void onSNIAttentionIconNameChanged(const QString &value);
    void onSNIAttentionIconPixmapChanged(DBusImageList value);
    void onSNIAttentionMovieNameChanged(const QString &value);
    void onSNICategoryChanged(const QString &value);
    void onSNIIconNameChanged(const QString &value);
    void onSNIIconPixmapChanged(DBusImageList  value);
    void onSNIIconThemePathChanged(const QString &value);
    void onSNIIdChanged(const QString &value);
    void onSNIMenuChanged(const QDBusObjectPath &value);
    void onSNIOverlayIconNameChanged(const QString &value);
    void onSNIOverlayIconPixmapChanged(DBusImageList  value);
    void onSNIStatusChanged(const QString &status);
    void hidePopup();
    void hideNonModel();
    void popupWindowAccept();
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    void paintEvent(QPaintEvent *e) override;
    QPixmap newIconPixmap(IconType iconType);
    void setMouseData(QMouseEvent *e);
    void handleMouseRelease();

private:
    StatusNotifierItem *m_sniInter;

    DBusMenuImporter *m_dbusMenuImporter;

    QMenu *m_menu;
    QTimer *m_updateIconTimer;
    QTimer *m_updateOverlayIconTimer;
    QTimer *m_updateAttentionIconTimer;

    QString m_sniServicePath;
    QString m_dbusService;
    QString m_dbusPath;

    QPixmap m_pixmap;
    QPixmap m_overlayPixmap;

    // SNI propertys
    QString m_sniAttentionIconName;
    DBusImageList m_sniAttentionIconPixmap;
    QString m_sniAttentionMovieName;
    QString m_sniCategory;
    QString m_sniIconName;
    DBusImageList m_sniIconPixmap;
    QString m_sniIconThemePath;
    QString m_sniId;
    QDBusObjectPath m_sniMenuPath;
    QString m_sniOverlayIconName;
    DBusImageList m_sniOverlayIconPixmap;
    QString m_sniStatus;
    QTimer *m_popupTipsDelayTimer;
    QTimer *m_handleMouseReleaseTimer;
    QPair<QPoint, Qt::MouseButton> m_lastMouseReleaseData;
    static Dock::Position DockPosition;
    static QPointer<DockPopupWindow> PopupWindow;
//    Dock::TipsWidget *m_tipsLabel;
    bool m_popupShown;
};

#endif /* SNIWIDGET_H */

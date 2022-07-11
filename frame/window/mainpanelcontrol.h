/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     xuwenw <xuwenw@xuwenw.so>
 *
 * Maintainer:  <@xuwenw.so>
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

#ifndef MAINPANELCONTROL_H
#define MAINPANELCONTROL_H

#include "constants.h"

#include <QWidget>

#include <com_deepin_daemon_gesture.h>

using namespace Dock;

class QBoxLayout;
class QLabel;
class TrayPluginItem;
class PluginsItem;
class DockItem;
class PlaceholderItem;
class AppDragWidget;
class DesktopWidget;
class TrayManagerWindow;
class DockScreen;
class RecentAppHelper;
class ToolAppHelper;

class MainPanelControl : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanelControl(QWidget *parent = nullptr);

    void setPositonValue(Position position);
    void setDisplayMode(DisplayMode dislayMode);
    void resizeDockIcon();
    void updatePluginsLayout();

    void setDockScreen(DockScreen *dockScreen);
    QPainterPath areaPath();
    QSize suitableSize(int screenSize, double deviceRatio) const;

public slots:
    void insertItem(const int index, DockItem *item);
    void removeItem(DockItem *item);
    void itemUpdated(DockItem *item);

signals:
    void itemMoved(DockItem *sourceItem, DockItem *targetItem);
    void itemAdded(const QString &appDesktop, int idx);
    void requestUpdate();

private:
    void initUI();
    void initConnection();
    void updateAppAreaSonWidgetSize();
    void updateMainPanelLayout();
    void updateDisplayMode();
    void moveAppSonWidget();
    void updateModeChange();

    void addFixedAreaItem(int index, QWidget *wdg);
    void removeFixedAreaItem(QWidget *wdg);
    void removeAppAreaItem(QWidget *wdg);
    void addTrayAreaItem(int index, QWidget *wdg);
    void removeTrayAreaItem(QWidget *wdg);
    int getScreenSize() const;

    // 拖拽相关
    void startDrag(DockItem *);
    DockItem *dropTargetItem(DockItem *sourceItem, QPoint point);
    void moveItem(DockItem *sourceItem, DockItem *targetItem);
    void handleDragMove(QDragMoveEvent *e, bool isFilter);
    void calcuDockIconSize(int w, int h, int traySize);
    void resizeDesktopWidget();
    bool checkNeedShowDesktop();
    bool appIsOnDock(const QString &appDesktop);
    void resetRadius();
    void dockRecentApp(DockItem *dockItem);

private Q_SLOTS:
    void onRequestUpdate();
    void onRecentVisibleChanged(bool visible);
    void onToolVisibleChanged(bool visible);

protected:
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void enterEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QBoxLayout *m_mainPanelLayout;

    QWidget *m_fixedAreaWidget;     // 固定区域
    QBoxLayout *m_fixedAreaLayout;  // 固定区域布局
    QLabel *m_fixedSpliter;         // 固定区域与应用区域间的分割线
    QWidget *m_appAreaWidget;       // 应用区域
    QWidget *m_appAreaSonWidget;    // 子应用区域，所在位置根据显示模式手动指定
    QBoxLayout *m_appAreaSonLayout; // 子应用区域布局
    QLabel *m_appSpliter;           // 应用区域与托盘区域间的分割线
    QWidget *m_trayAreaWidget;      // 托盘区域
    QBoxLayout *m_trayAreaLayout;   // 托盘区域布局
    QLabel *m_traySpliter;          // 托盘区域与插件区域间的分割线
    QWidget *m_pluginAreaWidget;    // 插件区域
    QWidget *m_recentAreaWidget;    // 最近打开应用
    QBoxLayout *m_recentLayout;
    QLabel *m_recentSpliter;        // 最近打开应用区域分割线
    QWidget *m_toolAreaWidget;      // 工具区域
    QBoxLayout *m_toolLayout;       // 工具区域布局

    TrayManagerWindow *m_trayManagerWidget;
    QBoxLayout *m_pluginLayout;     // 插件区域布局
    DesktopWidget *m_desktopWidget; // 桌面预览区域

    Position m_position;
    QPointer<PlaceholderItem> m_placeholderItem;
    QString m_draggingMimeKey;
    AppDragWidget *m_appDragWidget;
    DisplayMode m_displayMode;
    QPoint m_mousePressPos;
    TrayPluginItem *m_tray;
    int m_dragIndex = -1;           // 记录应用区域被拖拽图标的位置

    DockScreen *m_dockScreen;
    RecentAppHelper *m_recentHelper;
    ToolAppHelper *m_toolHelper;
};

#endif // MAINPANELCONTROL_H

// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef OVERFLOWITEM_H
#define OVERFLOWITEM_H

#include <QWidget>
#include "dockitem.h"


QT_USE_NAMESPACE
class QScrollArea;
class QBoxLayout;

class OverflowItem : public DockItem
{
    Q_OBJECT
public:
    explicit OverflowItem(QWidget *parent = nullptr);
    inline ItemType itemType() const override { return OverflowIcon; }
    void setPopUpSize(int width, int height);
    void addItem(QWidget *item);
    void hidePopUpWindow();
    void setLayoutPosition(Dock::Position position);
protected:
    void enterEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    bool eventFilter(QObject *watched, QEvent *e) override;
private:
    void paintEvent(QPaintEvent *e) override;
    //QWidget * popupTips() override;
    void showPopupWindow(QWidget *const content, const bool model = false, const int radius = 6) override;

private:
    QPoint OverflowIconPosition(const QPixmap &pixmap) const;
private:
    bool m_clicked;
    bool m_hover;
    bool m_showpopup;
    QScrollArea *m_scrollarea;
    QWidget *m_centerScroll;
    QBoxLayout *m_popuplayout;
    DockPopupWindow *m_popupwindow;
};

#endif // !OVERFLOWITEM_H

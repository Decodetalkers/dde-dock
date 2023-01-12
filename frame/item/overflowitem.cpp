#include "overflowitem.h"
#include "itemconsts.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
const QString image_src = QStringLiteral(":/icons/resources/application-x-desktop");

OverflowItem::OverflowItem(QWidget *parent)
    : DockItem(parent)
    , m_showpopup(false)
    , m_scrollarea(new QScrollArea)
    , m_centerScroll(new QWidget)
    , m_popuplayout(new QBoxLayout(QBoxLayout::BottomToTop))
    , m_popupwindow(new DockPopupWindow)
{
    m_popupwindow->setShadowBlurRadius(20);
    m_popupwindow->setRadius(6);
    m_popupwindow->setShadowYOffset(2);
    m_popupwindow->setShadowXOffset(0);
    m_popupwindow->setArrowWidth(18);
    m_popupwindow->setArrowHeight(10);
    m_popupwindow->setObjectName("overlaypopup");

    m_scrollarea->setFrameStyle(QFrame::NoFrame);
    m_scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setBackgroundRole(QPalette::Base);
    m_scrollarea->setWidgetResizable(true);

    m_centerScroll->setLayout(m_popuplayout);
    m_centerScroll->setAccessibleName(OVERFLOWWIDGET_ACCESS_NAME);
    m_centerScroll->setAttribute(Qt::WA_TranslucentBackground);
    m_scrollarea->setWidget(m_centerScroll);

    m_scrollarea->installEventFilter(this);
}

void OverflowItem::setPopUpSize(int width, int height) {
    m_scrollarea->setFixedSize(width, height);
}

void OverflowItem::addItem(QWidget *item) {
    m_popuplayout->addWidget(item,0, Qt::AlignCenter);
}

void OverflowItem::paintEvent(QPaintEvent *e) {

    DockItem::paintEvent(e);
    switch (DockPosition) {
        case Top:
        case Bottom:
            m_popuplayout->setDirection(QBoxLayout::LeftToRight);
            break;
        case Left:
        case Right:
            m_popuplayout->setDirection(QBoxLayout::BottomToTop);
            break;
    }
    if (!isVisible()) {
        return;
    }
    QPainter painter(this);

    QPixmap image(image_src);
    QRectF rf = QRectF(rect());
    QRectF rfp = QRectF(image.rect());
    QPointF p = rf.center() - rfp.center() / image.devicePixelRatioF();

    painter.drawPixmap(p, image);
}

QWidget *OverflowItem::popupTips() {
    return new QLabel("SSSS GRIDMAN");
}

void OverflowItem::mousePressEvent(QMouseEvent *e) {
    m_showpopup = !m_showpopup;
    if (m_showpopup) {
        showPopupWindow(m_scrollarea);
    } else {
        m_popupwindow->hide();
    }
    DockItem::mousePressEvent(e);
}

void OverflowItem::showPopupWindow(QWidget *const content, const bool model, const int radius) {
    PopupWindow->setRadius(radius);

    m_popupShown = true;
    m_lastPopupWidget = content;

    if (model)
        emit requestWindowAutoHide(false);

    DockPopupWindow *popup = m_popupwindow;
    QWidget *lastContent = popup->getContent();
    if (lastContent)
        lastContent->setVisible(false);

    switch (DockPosition) {
        case Top:   popup->setArrowDirection(DockPopupWindow::ArrowTop);     break;
        case Bottom: popup->setArrowDirection(DockPopupWindow::ArrowBottom);  break;
        case Left:  popup->setArrowDirection(DockPopupWindow::ArrowLeft);    break;
        case Right: popup->setArrowDirection(DockPopupWindow::ArrowRight);   break;
    }
    popup->resize(content->sizeHint());
    popup->setContent(content);

    const QPoint p = popupMarkPoint();
    if (!popup->isVisible())
        QMetaObject::invokeMethod(popup, "show", Qt::QueuedConnection, Q_ARG(QPoint, p), Q_ARG(bool, model));
    else
        popup->show(p, model);
}

bool OverflowItem::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_scrollarea && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        const QPoint delta = wheelEvent->angleDelta();
        int scroll_len = qAbs(delta.x()) > qAbs(delta.y()) ? delta.x() : delta.y(); 
        if (m_popuplayout->direction() == QBoxLayout::LeftToRight) {
            if (m_scrollarea->horizontalScrollBar()->value() + scroll_len <= 0) {
                m_scrollarea->horizontalScrollBar()->setValue(0);
            } else if (m_scrollarea->horizontalScrollBar()->value() + scroll_len >=
                       m_scrollarea->horizontalScrollBar()->maximum()) {
                m_scrollarea->horizontalScrollBar()->setValue(m_scrollarea->horizontalScrollBar()->maximum());
            } else {
                m_scrollarea->horizontalScrollBar()->setValue(m_scrollarea->horizontalScrollBar()->value() + scroll_len);
            }
        } else {
            if (m_scrollarea->verticalScrollBar()->value() + scroll_len <= 0) {
                m_scrollarea->verticalScrollBar()->setValue(0);
            } else if (m_scrollarea->verticalScrollBar()->value() + scroll_len >=
                       m_scrollarea->verticalScrollBar()->maximum()) {
                m_scrollarea->verticalScrollBar()->setValue(m_scrollarea->verticalScrollBar()->maximum());
            } else {
                m_scrollarea->verticalScrollBar()->setValue(m_scrollarea->verticalScrollBar()->value() + scroll_len);
            }
        }

        return true;
    }
    return DockItem::eventFilter(watched,event);

}

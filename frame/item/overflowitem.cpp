#include "overflowitem.h"
#include "dwindowmanagerhelper.h"
#include "itemconsts.h"
#include "appitem.h"
#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
const QString ICON_DEFAULT = QStringLiteral(":/icons/resources/application-x-desktop");
const QString OVERFLOW_MORE = QStringLiteral(":/icons/resources/overflow-more");

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
    m_popupwindow->setLeftRightRadius(true);

    m_scrollarea->setFrameStyle(QFrame::NoFrame);
    m_scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setBackgroundRole(QPalette::Base);
    m_scrollarea->setWidgetResizable(true);

    m_centerScroll->setLayout(m_popuplayout);
    m_centerScroll->setAccessibleName(OVERFLOWWIDGET_ACCESS_NAME);
    m_centerScroll->setAttribute(Qt::WA_TranslucentBackground);
    m_centerScroll->setAutoFillBackground(true);
    m_scrollarea->setWidget(m_centerScroll);

    m_scrollarea->installEventFilter(this);
}

void OverflowItem::hidePopUpWindow() {
    m_showpopup = false;
    m_popupwindow->hide();
}

void OverflowItem::setPopUpSize(int width, int height) {
    m_scrollarea->setFixedSize(width, height);
}

void OverflowItem::addItem(QWidget *item) {
    m_popuplayout->addWidget(item,0, Qt::AlignCenter);
}

QPoint OverflowItem::OverflowIconPosition(const QPixmap &pixmap) const {
    const auto ratio = devicePixelRatioF();
    const QRectF itemRect = rect();
    const QRectF iconRect = pixmap.rect();
    const qreal iconX = itemRect.center().x() - iconRect.center().x() / ratio;
    const qreal iconY = itemRect.center().y() - iconRect.center().y() / ratio;
    return QPoint(iconX, iconY);
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
            m_popuplayout->setDirection(QBoxLayout::TopToBottom);
            break;
    }
    if (!isVisible()) {
        return;
    }
    QPainter painter(this);

    QPixmap image(ICON_DEFAULT);
    if (m_popuplayout->count() != 0) {
        image = static_cast<AppItem *>(m_popuplayout->itemAt(0)->widget())->appIcon();
    }
    QPoint realsize = OverflowIconPosition(image);
    image.scaled(realsize.x(), realsize.y());
    painter.drawPixmap(realsize, image);

    painter.setOpacity(0.7);
    qreal min = qMin(rect().width(), rect().height());
    QRectF backgroundRect = QRectF(rect().x(), rect().y(), min, min);
    backgroundRect = backgroundRect.marginsRemoved(QMargins(2, 2, 2, 2));
    backgroundRect.moveCenter(rect().center());

    QPainterPath path;
    path.addRoundedRect(backgroundRect, 8, 8);
    painter.fillPath(path, QColor(0, 0, 0, 255 * 0.8));

    painter.setOpacity(1);
    QPixmap moreicons(OVERFLOW_MORE);
    QPoint realsize_more = OverflowIconPosition(moreicons);
    moreicons.scaled(realsize.x(), realsize.y());
    painter.drawPixmap(realsize_more, moreicons);

}


void OverflowItem::mousePressEvent(QMouseEvent *e) {
    m_showpopup = !m_showpopup;
    if (m_showpopup) {
        m_popupwindow->setLeftRightRadius(DWindowManagerHelper::instance()->hasComposite());
        showPopupWindow(m_scrollarea, true, 12);
    } else {
        m_popupwindow->hide();
    }
    DockItem::mousePressEvent(e);
}

void OverflowItem::showPopupWindow(QWidget *const content, const bool model, const int radius) {

    m_popupShown = true;
    m_lastPopupWidget = content;

    if (model)
        emit requestWindowAutoHide(false);

    m_popupwindow->setRadius(radius);
    QWidget *lastContent = m_popupwindow->getContent();
    if (lastContent)
        lastContent->setVisible(false);

    switch (DockPosition) {
        case Top:   m_popupwindow->setArrowDirection(DockPopupWindow::ArrowTop);     break;
        case Bottom: m_popupwindow->setArrowDirection(DockPopupWindow::ArrowBottom);  break;
        case Left:  m_popupwindow->setArrowDirection(DockPopupWindow::ArrowLeft);    break;
        case Right: m_popupwindow->setArrowDirection(DockPopupWindow::ArrowRight);   break;
    }
    m_popupwindow->resize(content->sizeHint());
    m_popupwindow->setContent(content);

    const QPoint p = popupMarkPoint();
    if (!m_popupwindow->isVisible())
        QMetaObject::invokeMethod(m_popupwindow, "show", Qt::QueuedConnection, Q_ARG(QPoint, p), Q_ARG(bool, model));
    else
        m_popupwindow->show(p, model);
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

        m_popupwindow->update();
        m_centerScroll->update();
        return true;
    }
    return DockItem::eventFilter(watched,event);

}

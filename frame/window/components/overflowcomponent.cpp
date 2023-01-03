#include "overflowcomponent.h"
#include "constants.h"
#include "dtkwidget_global.h"
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <DGuiApplicationHelper>
#include <QPainterPath>
#include <DStyle>
#include <algorithm>
DWIDGET_USE_NAMESPACE

OverFlowComponent::OverFlowComponent(QString imagesrc,QWidget *parent)
    : QWidget(parent),
      m_pimagesrc(imagesrc),
      m_hover(false),
      m_pressed(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void OverFlowComponent::mousePressEvent(QMouseEvent *event) {
    m_pressed = true;
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    update();
    QWidget::mouseMoveEvent(event);
}

void OverFlowComponent::mouseMoveEvent(QMouseEvent *event) {
    m_hover = true;
    update();
    QWidget::mouseMoveEvent(event);
}

void OverFlowComponent::mouseReleaseEvent(QMouseEvent *event) {
    m_hover = false;
    m_pressed = false;
    emit toggled();
    update();
    QWidget::mouseReleaseEvent(event);
}

void OverFlowComponent::enterEvent(QEvent *event) {
    m_hover = true;
    update();
    QWidget::enterEvent(event);
}

void OverFlowComponent::leaveEvent(QEvent *event) {
    m_hover = false;
    m_pressed = false;
    update();
    QWidget::leaveEvent(event);
}

void OverFlowComponent::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color = Qt::black;
    painter.setOpacity(0.5);

    if (m_hover) {
        painter.setOpacity(0.6);
    }

    if (m_pressed) {
        painter.setOpacity(0.3);
    }

    {
        QPainterPath path;
        DStyleHelper dstyle(style());
        const int radius = dstyle.pixelMetric(DStyle::PM_FrameRadius);

        int minSize = std::min(width() / 1.4 , height() / 1.4 );
        QRect rc(0,0, minSize, minSize);

        rc.moveTo(rect().center() - rc.center());

        path.addRoundedRect(rc, radius, radius);
        painter.fillPath(path, color);
    }

    painter.setOpacity(1);
    QPixmap image(m_pimagesrc);
    QRectF rf = QRectF(rect());
    QRectF rfp = QRectF(image.rect());
    QPointF p = rf.center() - rfp.center() / image.devicePixelRatioF();

    painter.drawPixmap(p, image);

}

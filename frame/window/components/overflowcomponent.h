#pragma once

#include <QWidget>

class OverFlowComponent : public QWidget {

    Q_OBJECT
public:
    explicit OverFlowComponent(QString imagesrc,QWidget *parent = nullptr);
    inline void setIcon(QString src) {
        m_pimagesrc = src;
        update();
    }
signals:
    void clicked();
    void toggled();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    QString m_pimagesrc;
    bool m_hover;
    bool m_pressed;

};

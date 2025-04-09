#pragma once
#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit CustomGraphicsView(QWidget* parent = nullptr);

protected:
    // Override to implement zooming via the scroll wheel.
    void wheelEvent(QWheelEvent* event) override;
    // Override to implement panning on middle mouse button press.
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool m_panning;
    QPoint m_lastMousePos;
};

#endif // CUSTOMGRAPHICSVIEW_H

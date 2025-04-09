#include "CustomGraphicsView.h"
#include <QScrollBar>

CustomGraphicsView::CustomGraphicsView(QWidget* parent)
    : QGraphicsView(parent), m_panning(false)
{
    // Ensure no built-in drag mode so we handle panning manually.
    setDragMode(QGraphicsView::NoDrag);
    // Enable interactive mode.
    setInteractive(true);
}

void CustomGraphicsView::wheelEvent(QWheelEvent* event)
{
    // Use a scaling factor for zooming.
    const double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        // Zoom in.
        scale(scaleFactor, scaleFactor);
    }
    else {
        // Zoom out.
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
    event->accept();
}

void CustomGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        // Start panning.
        m_panning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
    else {
        QGraphicsView::mousePressEvent(event);
    }
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning) {
        // Calculate the distance moved.
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        // Adjust the scrollbars accordingly.
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
    }
    else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }
    else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

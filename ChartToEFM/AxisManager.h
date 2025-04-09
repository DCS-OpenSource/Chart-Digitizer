#pragma once
#ifndef AXISMANAGER_H
#define AXISMANAGER_H

#include <QGraphicsLineItem>
#include <QVector>
#include <QPair>
#include <QPointF>

class QGraphicsScene;

class AxisManager
{
public:
    AxisManager();
    ~AxisManager();

    // Define or clear axes.
    void defineHorizontalAxis(const QPointF& start, const QPointF& end, QGraphicsScene* scene);
    void defineVerticalAxis(const QPointF& start, const QPointF& end, QGraphicsScene* scene);
    void clearHorizontalAxis(QGraphicsScene* scene);
    void clearVerticalAxis(QGraphicsScene* scene);

    // Access the current axis lines.
    QGraphicsLineItem* horizontalAxis() const;
    QGraphicsLineItem* verticalAxis() const;

    // Calibration: add a calibration point for each axis.
    void addHorizontalCalibration(const QPointF& pt, double value);
    void addVerticalCalibration(const QPointF& pt, double value);

    // Map a fractional position (0 to 1) to a real-world value using calibration.
    double mapHorizontal(double fraction, double endpointMin, double endpointMax) const;
    double mapVertical(double fraction, double endpointMin, double endpointMax) const;

private:
    // The drawn axis lines.
    QGraphicsLineItem* m_horizontalAxis;
    QGraphicsLineItem* m_verticalAxis;

    // Calibration data: each calibration point is stored as a pair (pixel coordinate, calibration value).
    QVector<QPair<QPointF, double>> m_horizontalCalib;
    QVector<QPair<QPointF, double>> m_verticalCalib;

    // Helper function to perform piecewise-linear mapping (similar to your mapCalibratedValue).
    double mapCalibratedValue(double fraction, double endpointMin, double endpointMax,
        const QVector<QPair<QPointF, double>>& calibPoints,
        const QPointF& A, const QPointF& B) const;
};

#endif // AXISMANAGER_H

#pragma once
#ifndef DATAPOINTMANAGER_H
#define DATAPOINTMANAGER_H

#include <QPointF>

class AxisManager;

class DataPointManager
{
public:
    // Requires a pointer or reference to an existing AxisManager.
    DataPointManager(const AxisManager* axisManager);

    // Map a screen point (in scene coordinates) to chart coordinates.
    // hMin, hMax, vMin, vMax should be taken from the endpoint calibration fields.
    void mapDataPoint(const QPointF& scenePoint,
        double hMin, double hMax, double vMin, double vMax,
        double& xValue, double& yValue) const;

private:
    const AxisManager* m_axisManager;
};

#endif // DATAPOINTMANAGER_H

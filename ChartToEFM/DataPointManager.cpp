#include "DataPointManager.h"
#include "AxisManager.h"
#include <QGraphicsLineItem>
#include <QLineF>
#include <QtMath>

DataPointManager::DataPointManager(const AxisManager* axisManager)
    : m_axisManager(axisManager)
{
}

void DataPointManager::mapDataPoint(const QPointF& scenePoint,
    double hMin, double hMax,
    double vMin, double vMax,
    double& xValue, double& yValue) const
{
    QGraphicsLineItem* hLineItem = m_axisManager->horizontalAxis();
    QGraphicsLineItem* vLineItem = m_axisManager->verticalAxis();
    if (!hLineItem || !vLineItem) {
        xValue = hMin;
        yValue = vMin;
        return;
    }
    QLineF hLine = hLineItem->line();
    QLineF vLine = vLineItem->line();

    double normSq_h = qPow(hLine.p2().x() - hLine.p1().x(), 2) + qPow(hLine.p2().y() - hLine.p1().y(), 2);
    double normSq_v = qPow(vLine.p2().x() - vLine.p1().x(), 2) + qPow(vLine.p2().y() - vLine.p1().y(), 2);

    double hFraction = 0.0;
    if (normSq_h != 0) {
        QLineF lineH(hLine.p1(), scenePoint);
        hFraction = (lineH.dx() * hLine.dx() + lineH.dy() * hLine.dy()) / normSq_h;
    }

    double vFraction = 0.0;
    if (normSq_v != 0) {
        QLineF lineV(vLine.p1(), scenePoint);
        vFraction = (lineV.dx() * vLine.dx() + lineV.dy() * vLine.dy()) / normSq_v;
    }

    xValue = m_axisManager->mapHorizontal(hFraction, hMin, hMax);
    yValue = m_axisManager->mapVertical(vFraction, vMin, vMax);
}

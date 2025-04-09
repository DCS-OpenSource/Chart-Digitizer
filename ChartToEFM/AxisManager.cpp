#include "AxisManager.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QtMath>
#include <algorithm>

AxisManager::AxisManager()
    : m_horizontalAxis(nullptr),
    m_verticalAxis(nullptr)
{
}

AxisManager::~AxisManager()
{
}

void AxisManager::defineHorizontalAxis(const QPointF& start, const QPointF& end, QGraphicsScene* scene)
{
    clearHorizontalAxis(scene);
    m_horizontalAxis = scene->addLine(QLineF(start, end));
}

void AxisManager::defineVerticalAxis(const QPointF& start, const QPointF& end, QGraphicsScene* scene)
{
    clearVerticalAxis(scene);
    m_verticalAxis = scene->addLine(QLineF(start, end));
}

void AxisManager::clearHorizontalAxis(QGraphicsScene* scene)
{
    if (m_horizontalAxis) {
        scene->removeItem(m_horizontalAxis);
        delete m_horizontalAxis;
        m_horizontalAxis = nullptr;
    }
}

void AxisManager::clearVerticalAxis(QGraphicsScene* scene)
{
    if (m_verticalAxis) {
        scene->removeItem(m_verticalAxis);
        delete m_verticalAxis;
        m_verticalAxis = nullptr;
    }
}

QGraphicsLineItem* AxisManager::horizontalAxis() const
{
    return m_horizontalAxis;
}

QGraphicsLineItem* AxisManager::verticalAxis() const
{
    return m_verticalAxis;
}

void AxisManager::addHorizontalCalibration(const QPointF& pt, double value)
{
    m_horizontalCalib.append(qMakePair(pt, value));
}

void AxisManager::addVerticalCalibration(const QPointF& pt, double value)
{
    m_verticalCalib.append(qMakePair(pt, value));
}

double AxisManager::mapCalibratedValue(double fraction, double endpointMin, double endpointMax,
    const QVector<QPair<QPointF, double>>& calibPoints,
    const QPointF& A, const QPointF& B) const
{
    QVector<QPair<double, double>> nodes;
    nodes.append(qMakePair(0.0, endpointMin));
    nodes.append(qMakePair(1.0, endpointMax));
    double normSq = qPow(B.x() - A.x(), 2) + qPow(B.y() - A.y(), 2);
    if (normSq <= 0.0)
        return endpointMin;
    for (const auto& calib : calibPoints) {
        QPointF AP = calib.first - A;
        QPointF AB = B - A;
        double t = (AP.x() * AB.x() + AP.y() * AB.y()) / normSq;
        nodes.append(qMakePair(t, calib.second));
    }
    std::sort(nodes.begin(), nodes.end(), [](const QPair<double, double>& a, const QPair<double, double>& b) {
        return a.first < b.first;
        });
    if (fraction <= nodes.first().first)
        return nodes.first().second;
    if (fraction >= nodes.last().first)
        return nodes.last().second;
    for (int i = 0; i < nodes.size() - 1; ++i) {
        double t0 = nodes[i].first;
        double t1 = nodes[i + 1].first;
        if (fraction >= t0 && fraction <= t1) {
            double v0 = nodes[i].second;
            double v1 = nodes[i + 1].second;
            double rel = (fraction - t0) / (t1 - t0);
            return v0 + rel * (v1 - v0);
        }
    }
    return endpointMin;
}

double AxisManager::mapHorizontal(double fraction, double endpointMin, double endpointMax) const
{
    if (!m_horizontalAxis)
        return endpointMin;
    QPointF A = m_horizontalAxis->line().p1();
    QPointF B = m_horizontalAxis->line().p2();
    return mapCalibratedValue(fraction, endpointMin, endpointMax, m_horizontalCalib, A, B);
}

double AxisManager::mapVertical(double fraction, double endpointMin, double endpointMax) const
{
    if (!m_verticalAxis)
        return endpointMin;
    QPointF A = m_verticalAxis->line().p1();
    QPointF B = m_verticalAxis->line().p2();
    return mapCalibratedValue(fraction, endpointMin, endpointMax, m_verticalCalib, A, B);
}

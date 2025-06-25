#include "plotmodel.h"
#include "functionevaluator.h"
#include <QVariantMap>

PlotModel::PlotModel(QObject *parent) : QObject(parent) {}

QVariantList PlotModel::generatePoints(const QString &expression, const double xMin, const double xMax, const int numPoints)
{
    QVariantList result;
    
    const QList<QPointF> points = FunctionEvaluator::generatePoints(expression, xMin, xMax, numPoints);
    
    for (const QPointF &point : points) {
        QVariantMap map;
        map["x"] = point.x();
        map["y"] = point.y();
        result.append(map);
    }
    
    return result;
}
#ifndef PLOTMODEL_H
#define PLOTMODEL_H

#include <QVariantList>

class PlotModel final : public QObject
{
    Q_OBJECT

public:
    explicit PlotModel(QObject *parent = nullptr);

    Q_INVOKABLE static QVariantList generatePoints(const QString &expression, double xMin, double xMax, int numPoints);
};

#endif // PLOTMODEL_H
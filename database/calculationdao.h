#ifndef CALCULATIONDAO_H
#define CALCULATIONDAO_H

#include "calculationentity.h"

class CalculationDAO final : public QObject
{
    Q_OBJECT

public:
    explicit CalculationDAO(QObject *parent = nullptr);

    static bool save(CalculationEntity &calculation);

    static CalculationEntity findById(int id);

    static QList<CalculationEntity> findAll();

    static bool deleteById(int id);

    static bool deleteAll();

private:
    static CalculationEntity mapFromQuery(const class QSqlQuery &query);
};

#endif // CALCULATIONDAO_H

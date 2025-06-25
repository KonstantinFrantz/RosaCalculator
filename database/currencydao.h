#ifndef CURRENCYDAO_H
#define CURRENCYDAO_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include "currencyentity.h"

class CurrencyDAO final : public QObject
{
    Q_OBJECT

public:
    explicit CurrencyDAO(QObject *parent = nullptr);

    static bool saveAll(const QList<CurrencyEntity> &currencies);
    static QList<CurrencyEntity> findAll();
    static QDateTime getLastUpdateTime();

private:
    static CurrencyEntity mapFromQuery(const class QSqlQuery &query);
};

#endif // CURRENCYDAO_H

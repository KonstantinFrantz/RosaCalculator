#include "currencydao.h"
#include "currencyentity.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

CurrencyDAO::CurrencyDAO(QObject *parent)
    : QObject(parent)
{
}

bool CurrencyDAO::saveAll(const QList<CurrencyEntity> &currencies)
{
    if (!DatabaseManager::instance().isInitialized()) {
        return false;
    }

    QSqlDatabase db = DatabaseManager::database();
    db.transaction();

    if (QSqlQuery deleteQuery(db); !deleteQuery.exec("DELETE FROM currency_rates")) {
        qCritical() << "Ошибка очистки таблицы курсов:" << deleteQuery.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare(R"(
        INSERT INTO currency_rates
        (code, name, nominal, rate, last_update)
        VALUES (:code, :name, :nominal, :rate, :last_update)
    )");

    for (const auto &currency : currencies) {
        insertQuery.bindValue(":code", currency.code());
        insertQuery.bindValue(":name", currency.name());
        insertQuery.bindValue(":nominal", currency.nominal());
        insertQuery.bindValue(":rate", currency.rate());
        insertQuery.bindValue(":last_update", currency.lastUpdate());

        if (!insertQuery.exec()) {
            qCritical() << "Ошибка вставки курса валюты:" << insertQuery.lastError().text();
            db.rollback();
            return false;
        }
    }

    return db.commit();
}

QList<CurrencyEntity> CurrencyDAO::findAll()
{
    QList<CurrencyEntity> currencies;

    if (!DatabaseManager::instance().isInitialized()) {
        return currencies;
    }

    QSqlQuery query(DatabaseManager::database());

    if (!query.exec(R"(
        SELECT id, code, name, nominal, rate, last_update
        FROM currency_rates
        ORDER BY code
    )")) {
        qCritical() << "Ошибка получения списка курсов:" << query.lastError().text();
        return currencies;
    }

    while (query.next()) {
        currencies.append(mapFromQuery(query));
    }

    return currencies;
}

QDateTime CurrencyDAO::getLastUpdateTime()
{
    if (!DatabaseManager::instance().isInitialized()) {
        return {};
    }

    QSqlQuery query(DatabaseManager::database());

    if (!query.exec("SELECT MAX(last_update) FROM currency_rates")) {
        qCritical() << "Ошибка получения времени обновления:" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        return query.value(0).toDateTime();
    }

    return {};
}

CurrencyEntity CurrencyDAO::mapFromQuery(const QSqlQuery &query)
{
    return {
        query.value("id").toInt(),
        query.value("code").toString(),
        query.value("name").toString(),
        query.value("nominal").toInt(),
        query.value("rate").toDouble(),
        query.value("last_update").toDateTime()
    };
}

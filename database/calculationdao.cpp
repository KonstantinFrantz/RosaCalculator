#include "calculationdao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

CalculationDAO::CalculationDAO(QObject *parent)
    : QObject(parent)
{
}

bool CalculationDAO::save(CalculationEntity &calculation)
{
    if (!DatabaseManager::instance().isInitialized()) {
        qWarning() << "База данных не инициализирована";
        return false;
    }

    QSqlQuery query(DatabaseManager::database());

    query.prepare("INSERT INTO calculations (expression, result, timestamp) VALUES (:expression, :result, :timestamp)");
    query.bindValue(":expression", calculation.expression());
    query.bindValue(":result", calculation.result());
    query.bindValue(":timestamp", calculation.timestamp());

    if (!query.exec()) {
        qCritical() << "Ошибка сохранения расчета:" << query.lastError().text();
        return false;
    }

    calculation.setId(query.lastInsertId().toInt());

    return true;
}

CalculationEntity CalculationDAO::findById(const int id)
{
    if (!DatabaseManager::instance().isInitialized()) {
        return {};
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT id, expression, result, timestamp FROM calculations WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Ошибка поиска расчета по ID:" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        return mapFromQuery(query);
    }

    return {};
}

QList<CalculationEntity> CalculationDAO::findAll()
{
    QList<CalculationEntity> calculations;

    if (!DatabaseManager::instance().isInitialized()) {
        return calculations;
    }

    QSqlQuery query(DatabaseManager::database());

    if (!query.exec("SELECT id, expression, result, timestamp FROM calculations ORDER BY timestamp DESC")) {
        qCritical() << "Ошибка получения списка расчетов:" << query.lastError().text();
        return calculations;
    }

    while (query.next()) {
        calculations.append(mapFromQuery(query));
    }

    return calculations;
}

bool CalculationDAO::deleteById(const int id)
{
    if (!DatabaseManager::instance().isInitialized()) {
        return false;
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare("DELETE FROM calculations WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Ошибка удаления расчета:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool CalculationDAO::deleteAll()
{
    if (!DatabaseManager::instance().isInitialized()) {
        return false;
    }

    if (QSqlQuery query(DatabaseManager::database()); !query.exec("DELETE FROM calculations")) {
        qCritical() << "Ошибка очистки истории расчетов:" << query.lastError().text();
        return false;
    }

    return true;
}

CalculationEntity CalculationDAO::mapFromQuery(const QSqlQuery &query)
{
    return {
        query.value("id").toInt(),
        query.value("expression").toString(),
        query.value("result").toString(),
        query.value("timestamp").toDateTime()
        };
}

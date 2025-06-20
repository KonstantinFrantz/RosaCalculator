#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>

const QString DatabaseManager::CONNECTION_NAME = "CalculatorDB";

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString &databasePath)
{
    if (m_initialized) {
        return true;
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical() << "Не удалось открыть базу данных:" << m_database.lastError().text();
        return false;
    }

    if (!createTables()) {
        qCritical() << "Не удалось создать таблицы";
        return false;
    }

    m_initialized = true;
    return true;
}

QSqlDatabase DatabaseManager::database() {
    return QSqlDatabase::database(CONNECTION_NAME);
}

void DatabaseManager::close()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
    m_initialized = false;
}

bool DatabaseManager::isInitialized() const
{
    return m_initialized;
}

bool DatabaseManager::createTables() const {
    const QString createCalculationsTable = R"(
        CREATE TABLE IF NOT EXISTS calculations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            expression TEXT NOT NULL,
            result TEXT NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    const QString createIndexQuery = R"(
        CREATE INDEX IF NOT EXISTS idx_calculations_timestamp
        ON calculations(timestamp DESC)
    )";

    return executeQuery(createCalculationsTable) && executeQuery(createIndexQuery);
}

bool DatabaseManager::executeQuery(const QString &query) const {
    if (QSqlQuery sqlQuery(m_database); !sqlQuery.exec(query)) {
        qCritical() << "Ошибка выполнения запроса:" << sqlQuery.lastError().text();
        qCritical() << "Запрос:" << query;
        return false;
    }
    return true;
}

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager final : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();

    bool initialize(const QString &databasePath);

    static QSqlDatabase database();

    void close();

    [[nodiscard]] bool isInitialized() const;

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    ~DatabaseManager() override;

    [[nodiscard]] bool createTables() const;

    [[nodiscard]] bool executeQuery(const QString &query) const;

    QSqlDatabase m_database;

    bool m_initialized;

    static const QString CONNECTION_NAME;
};

#endif // DATABASEMANAGER_H

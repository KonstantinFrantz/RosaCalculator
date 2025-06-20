#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QAbstractListModel>
#include "database/calculationentity.h"
#include "database/calculationdao.h"

class HistoryManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles { ExpressionRole = Qt::UserRole + 1,
                 ResultRole};

    explicit HistoryManager(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data (const QModelIndex &index, int role) const override;

    [[nodiscard]] QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE [[nodiscard]] int count() const;
    Q_INVOKABLE void addCalculation(const QString &expr,const QString &res);
    Q_INVOKABLE void clearHistory();
    Q_INVOKABLE void removeCalculation(int index);

signals:
    void countChanged();

private:
    void refresh();

    QList<CalculationEntity> m_calculations;
};

#endif // HISTORYMANAGER_H

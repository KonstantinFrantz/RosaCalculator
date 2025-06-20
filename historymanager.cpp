#include "historymanager.h"
#include <QDateTime>
#include <QRegularExpression>

HistoryManager::HistoryManager(QObject *parent)
    : QAbstractListModel(parent)
{
    refresh();
}

int HistoryManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_calculations.size();
}

QVariant HistoryManager::data(const QModelIndex &index, const int role) const
{
    if(!index.isValid() || index.row()>=m_calculations.size()) return {};
    const CalculationEntity &c = m_calculations[index.row()];

    switch(role){
        case ExpressionRole:
            return c.expression();
        case ResultRole:
            return c.result();
        default:
            return {};
    }
}

QHash<int,QByteArray> HistoryManager::roleNames() const{
    return {
        {ExpressionRole,"expression"},
        {ResultRole,"result"}
    };
}

int HistoryManager::count() const { return m_calculations.size(); }

void HistoryManager::addCalculation(const QString &expr,const QString &res)
{
    static const QRegularExpression opRe("[+\\-*/×÷^%costansin!sqrtlnlog]");
    if(!opRe.match(expr).hasMatch()) return;

    CalculationEntity calculation;
    calculation.setExpression(expr);
    calculation.setResult(res);
    calculation.setTimestamp(QDateTime::currentDateTime());

    if(CalculationDAO::save(calculation)){
        beginInsertRows({},0,0);
        m_calculations.prepend(calculation);
        endInsertRows();
        emit countChanged();
    }
}

void HistoryManager::clearHistory()
{
    if(CalculationDAO::deleteAll()){
        beginResetModel();
        m_calculations.clear();
        endResetModel();
        emit countChanged();
    }
}

void HistoryManager::removeCalculation(int index)
{
    if(index<0 || index>=m_calculations.size()) return;
    if(CalculationDAO::deleteById(m_calculations[index].id())){
        beginRemoveRows({},index,index);
        m_calculations.removeAt(index);
        endRemoveRows();
        emit countChanged();
    }
}

void HistoryManager::refresh()
{
    beginResetModel();
    m_calculations = CalculationDAO::findAll();
    endResetModel();
    emit countChanged();
}

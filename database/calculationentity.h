#ifndef CALCULATIONENTITY_H
#define CALCULATIONENTITY_H

#include <QDateTime>

class CalculationEntity
{
public:
    CalculationEntity();
    CalculationEntity(int id, QString expression, QString result, QDateTime timestamp);

    [[nodiscard]] int id() const;
    void setId(int id);

    [[nodiscard]] QString expression() const;
    void setExpression(const QString &expression);

    [[nodiscard]] QString result() const;
    void setResult(const QString &result);

    [[nodiscard]] QDateTime timestamp() const;
    void setTimestamp(const QDateTime &timestamp);

    [[nodiscard]] bool isValid() const;

private:
    int m_id;
    QString m_expression;
    QString m_result;
    QDateTime m_timestamp;
};

#endif // CALCULATIONENTITY_H

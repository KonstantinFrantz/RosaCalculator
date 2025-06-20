#include "calculationentity.h"

#include <utility>

CalculationEntity::CalculationEntity()
    : m_id(-1) {
}

CalculationEntity::CalculationEntity(const int id, QString expression, QString result, QDateTime timestamp)
    : m_id(id), m_expression(std::move(expression)), m_result(std::move(result)), m_timestamp(std::move(timestamp)) {
}

int CalculationEntity::id() const { return m_id; }

void CalculationEntity::setId(const int id) { m_id = id; }

QString CalculationEntity::expression() const { return m_expression; }

void CalculationEntity::setExpression(const QString &expression) { m_expression = expression; }

QString CalculationEntity::result() const { return m_result; }

void CalculationEntity::setResult(const QString &result) { m_result = result; }

QDateTime CalculationEntity::timestamp() const { return m_timestamp; }

void CalculationEntity::setTimestamp(const QDateTime &timestamp) { m_timestamp = timestamp; }

bool CalculationEntity::isValid() const {
    return !m_expression.isEmpty() && !m_result.isEmpty() && m_timestamp.isValid();
}

#include "currencyentity.h"

#include <utility>

CurrencyEntity::CurrencyEntity()
    : m_id(-1), m_nominal(1), m_rate(0.0)
{
}

CurrencyEntity::CurrencyEntity(const int id, QString code, QString name,
                               const int nominal, const double rate, QDateTime lastUpdate)
    : m_id(id), m_code(std::move(code)), m_name(std::move(name)), m_nominal(nominal),
      m_rate(rate), m_lastUpdate(std::move(lastUpdate))
{
}

int CurrencyEntity::id() const { return m_id; }

void CurrencyEntity::setId(const int id) { m_id = id; }

QString CurrencyEntity::code() const { return m_code; }

void CurrencyEntity::setCode(const QString &code) { m_code = code; }

QString CurrencyEntity::name() const { return m_name; }

void CurrencyEntity::setName(const QString &name) { m_name = name; }

int CurrencyEntity::nominal() const { return m_nominal; }

void CurrencyEntity::setNominal(const int nominal) { m_nominal = nominal; }

double CurrencyEntity::rate() const { return m_rate; }

void CurrencyEntity::setRate(const double rate) { m_rate = rate; }

QDateTime CurrencyEntity::lastUpdate() const { return m_lastUpdate; }

void CurrencyEntity::setLastUpdate(const QDateTime &lastUpdate) { m_lastUpdate = lastUpdate; }

bool CurrencyEntity::isValid() const
{
    return !m_code.isEmpty() && !m_name.isEmpty() && m_rate > 0 && m_lastUpdate.isValid();
}

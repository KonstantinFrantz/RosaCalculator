#ifndef CURRENCYENTITY_H
#define CURRENCYENTITY_H

#include <QDateTime>
#include <QString>

class CurrencyEntity
{
public:
    CurrencyEntity();
    CurrencyEntity(int id, QString code, QString name,
                   int nominal, double rate, QDateTime lastUpdate);

    [[nodiscard]] int id() const;
    void setId(int id);

    [[nodiscard]] QString code() const;
    void setCode(const QString &code);

    [[nodiscard]] QString name() const;
    void setName(const QString &name);

    [[nodiscard]] int nominal() const;
    void setNominal(int nominal);

    [[nodiscard]] double rate() const;
    void setRate(double rate);

    [[nodiscard]] QDateTime lastUpdate() const;
    void setLastUpdate(const QDateTime &lastUpdate);

    [[nodiscard]] bool isValid() const;

private:
    int m_id;
    QString m_code;
    QString m_name;
    int m_nominal;
    double m_rate;
    QDateTime m_lastUpdate;
};

#endif // CURRENCYENTITY_H
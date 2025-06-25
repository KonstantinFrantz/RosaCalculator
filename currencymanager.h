#ifndef CURRENCYMANAGER_H
#define CURRENCYMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

struct CurrencyRate {
    QString code;
    QString name;
    int nominal;
    double rate;
    QDateTime lastUpdate;
};

class CurrencyManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList currencies READ currencies NOTIFY currenciesChanged)

public:
    explicit CurrencyManager(QObject *parent = nullptr);
    ~CurrencyManager() override;

    void initialize();

    Q_INVOKABLE double convert(const QString &fromCurrency, const QString &toCurrency, double amount);
    Q_INVOKABLE [[nodiscard]] QString getCurrencyName(const QString &code) const;

    [[nodiscard]] QStringList currencies() const;
    [[nodiscard]] bool hasRates() const { return !m_rates.isEmpty(); }

signals:
    void currenciesChanged();
    void errorOccurred(const QString &error);

private slots:
    void onNetworkReply(QNetworkReply *reply);

private:
    void loadRatesFromDatabase();
    void loadRatesFromNetwork() const;
    void parseXmlData(const QByteArray &data);
    void saveRatesToDatabase() const;

    QNetworkAccessManager *m_networkManager;
    QMap<QString, CurrencyRate> m_rates;

    static const QString CBR_API_URL;
};

#endif // CURRENCYMANAGER_H

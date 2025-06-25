#include "currencymanager.h"
#include "database/currencydao.h"
#include "database/currencyentity.h"
#include <QNetworkRequest>
#include <QXmlStreamReader>
#include <QDebug>

const QString CurrencyManager::CBR_API_URL = "https://www.cbr-xml-daily.ru/daily_utf8.xml";

CurrencyManager::CurrencyManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &CurrencyManager::onNetworkReply);
}

CurrencyManager::~CurrencyManager() = default;

void CurrencyManager::initialize()
{
    loadRatesFromDatabase();
    if (!hasRates()) {
        loadRatesFromNetwork();
    }
}

void CurrencyManager::loadRatesFromNetwork() const {
    const QNetworkRequest request{QUrl(CBR_API_URL)};
    m_networkManager->get(request);
}

void CurrencyManager::onNetworkReply(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Ошибка загрузки курсов:" << reply->errorString();
        emit errorOccurred(reply->errorString());
        return;
    }

    const QByteArray data = reply->readAll();
    parseXmlData(data);
}

void CurrencyManager::parseXmlData(const QByteArray &data)
{
    m_rates.clear();

    CurrencyRate rub;
    rub.code = "RUB";
    rub.name = "Российский рубль";
    rub.nominal = 1;
    rub.rate = 1.0;
    rub.lastUpdate = QDateTime::currentDateTime();
    m_rates[rub.code] = rub;

    QXmlStreamReader xml(data);
    CurrencyRate currentRate;

    while (!xml.atEnd() && !xml.hasError()) {
        const QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument) continue;

        if (token == QXmlStreamReader::StartElement) {
            if (const QStringView name = xml.name(); name == u"Valute") {
                currentRate = CurrencyRate();
                currentRate.lastUpdate = QDateTime::currentDateTime();
            }
            else if (name == u"CharCode") {
                currentRate.code = xml.readElementText();
            }
            else if (name == u"Name") {
                currentRate.name = xml.readElementText();
            }
            else if (name == u"Nominal") {
                currentRate.nominal = xml.readElementText().toInt();
            }
            else if (name == u"Value") {
                QString value = xml.readElementText();
                value.replace(',', '.');
                currentRate.rate = value.toDouble();
            }
        }
        else if (token == QXmlStreamReader::EndElement && xml.name() == u"Valute") {
            if (!currentRate.code.isEmpty() && currentRate.rate > 0) {
                m_rates[currentRate.code] = currentRate;
            }
        }
    }

    if (xml.hasError()) {
        qWarning() << "Ошибка парсинга XML:" << xml.errorString();
        emit errorOccurred("Ошибка парсинга данных");
        return;
    }

    if (!m_rates.isEmpty()) {
        saveRatesToDatabase();
        emit currenciesChanged();
    }
}

void CurrencyManager::loadRatesFromDatabase()
{
    const QList<CurrencyEntity> entities = CurrencyDAO::findAll();
    m_rates.clear();

    for (const auto &entity : entities) {
        CurrencyRate rate;
        rate.code = entity.code();
        rate.name = entity.name();
        rate.nominal = entity.nominal();
        rate.rate = entity.rate();
        rate.lastUpdate = entity.lastUpdate();
        m_rates[rate.code] = rate;
    }

    if (!m_rates.isEmpty()) {
        emit currenciesChanged();
    }
}

void CurrencyManager::saveRatesToDatabase() const {
    QList<CurrencyEntity> entities;

    for (auto it = m_rates.constBegin(); it != m_rates.constEnd(); ++it) {
        const auto &[code, name, nominal, rate, lastUpdate] = it.value();
        entities.append(CurrencyEntity(
            -1, code, name, nominal, rate, lastUpdate
        ));
    }

    CurrencyDAO::saveAll(entities);
}

double CurrencyManager::convert(const QString &fromCurrency, const QString &toCurrency, const double amount)
{
    if (!m_rates.contains(fromCurrency) || !m_rates.contains(toCurrency)) {
        return 0.0;
    }

    const CurrencyRate &fromRate = m_rates[fromCurrency];
    const CurrencyRate &toRate = m_rates[toCurrency];

    const double inRubles = amount * fromRate.rate / fromRate.nominal;
    return inRubles * toRate.nominal / toRate.rate;
}

QString CurrencyManager::getCurrencyName(const QString &code) const
{
    if (m_rates.contains(code)) {
        return m_rates[code].name;
    }
    return {};
}

QStringList CurrencyManager::currencies() const
{
    return m_rates.keys();
}

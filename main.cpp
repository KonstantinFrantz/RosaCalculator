#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>

#include "calculator.h"
#include "historymanager.h"
#include "plotmodel.h"
#include "currencymanager.h"
#include "database/databasemanager.h"

int main(int argc, char *argv[])
{
    const QApplication app(argc, argv);

    QApplication::setApplicationName("Calculator");

    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/calculator.db";

    DatabaseManager::instance().initialize(dbPath);

    Calculator calculator;
    HistoryManager historyManager;
    PlotModel plotModel;
    CurrencyManager currencyManager;

    currencyManager.initialize();

    QQmlApplicationEngine engine;

    qmlRegisterType<Calculator>("CalculatorModule", 1, 0, "Calculator");
    qmlRegisterType<HistoryManager>("CalculatorModule", 1, 0, "HistoryManager");
    qmlRegisterType<CurrencyManager>("CalculatorModule", 1, 0, "CurrencyManager");

    engine.rootContext()->setContextProperty("calculator", &calculator);
    engine.rootContext()->setContextProperty("historyManager", &historyManager);
    engine.rootContext()->setContextProperty("plotModel", &plotModel);
    engine.rootContext()->setContextProperty("currencyManager", &currencyManager);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](const QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return QApplication::exec();
}

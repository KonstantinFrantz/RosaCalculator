#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>

#include "calculator.h"
#include "historymanager.h"
#include "database/databasemanager.h"

int main(int argc, char *argv[])
{
    const QGuiApplication app(argc, argv);

    QGuiApplication::setApplicationName("Calculator");

    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/calculator.db";

    DatabaseManager::instance().initialize(dbPath);

    Calculator calculator;
    HistoryManager historyManager;

    QQmlApplicationEngine engine;

    qmlRegisterType<Calculator>("CalculatorModule", 1, 0, "Calculator");
    qmlRegisterType<HistoryManager>("CalculatorModule", 1, 0, "HistoryManager");

    engine.rootContext()->setContextProperty("calculator", &calculator);
    engine.rootContext()->setContextProperty("historyManager", &historyManager);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](const QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return QGuiApplication::exec();
}

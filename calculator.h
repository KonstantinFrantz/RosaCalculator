#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>

class Calculator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString expression READ expression NOTIFY expressionChanged)
    Q_PROPERTY(QString previewResult READ previewResult NOTIFY previewResultChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(bool angleDegrees READ angleDegrees NOTIFY angleModeChanged)
    Q_PROPERTY(bool hasVariable READ hasVariable NOTIFY hasVariableChanged)

public:
    explicit Calculator(QObject *parent = nullptr);

    [[nodiscard]] QString expression() const;
    [[nodiscard]] QString previewResult() const;
    [[nodiscard]] bool angleDegrees() const;
    [[nodiscard]] bool hasError() const;
    [[nodiscard]] QString errorMessage() const;
    [[nodiscard]] bool hasVariable() const;

signals:
    void expressionChanged();
    void previewResultChanged();
    void errorChanged();
    void calculationPerformed(const QString &expr, const QString &result);
    void angleModeChanged();
    void hasVariableChanged();
    void plotRequested(const QString &expr);

public slots:
    void appendDigit(const QString &d);
    void appendOperator(const QString &op);
    void appendDot();
    void deleteLast();
    void clear();
    void evaluateResult();
    void evaluatePreview();

    Q_INVOKABLE void calculate();
    Q_INVOKABLE void backspace();
    Q_INVOKABLE void addDecimalPoint();
    Q_INVOKABLE void percentage();
    Q_INVOKABLE void appendFunction(const QString &func);
    Q_INVOKABLE void appendConstant(const QString &symbol);
    Q_INVOKABLE void appendVariable();
    Q_INVOKABLE void addParenthesis();
    Q_INVOKABLE void sqrt();
    Q_INVOKABLE void power();
    Q_INVOKABLE void factorial();
    Q_INVOKABLE void loadExpression(const QString &expr);
    Q_INVOKABLE void toggleAngleMode();
    Q_INVOKABLE void requestPlot();

private:
    void setCurrentNumber(const QString &d);
    void setExpression(const QString &e);
    void clearError();
    void checkForVariable();
    static QString prepareExpression(const QString &raw);
    static QString formatResult(double res);

    QString m_currentNumber;
    QString m_expression;
    QString m_previewResult;
    QString m_errorMessage;

    bool m_hasError = false;
    bool m_lastWasResult = false;
    bool m_angleDegrees = false;
    bool m_hasVariable = false;
    int m_openParensCount = 0;
};

#endif // CALCULATOR_H

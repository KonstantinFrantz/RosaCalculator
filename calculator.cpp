#include "calculator.h"
#include "parser.h"
#include "arithmeticevaluator.h"
#include "functionevaluator.h"

#include <QRegularExpression>
#include <QDebug>

static const QRegularExpression trailingOps("[+\\-*/^%]|\\.$");

Calculator::Calculator(QObject *parent) : QObject(parent) {}

QString Calculator::expression() const { return m_expression; }
QString Calculator::previewResult() const { return m_previewResult; }
bool Calculator::hasError() const { return m_hasError; }
bool Calculator::angleDegrees() const { return m_angleDegrees; }
QString Calculator::errorMessage() const { return m_errorMessage; }
bool Calculator::hasVariable() const { return m_hasVariable; }

void Calculator::setCurrentNumber(const QString &d) {
    if (m_currentNumber != d) {
        m_currentNumber = d;
    }
}

void Calculator::setExpression(const QString &e) {
    if (m_expression != e) {
        m_expression = e;
        emit expressionChanged();
        checkForVariable();
    }
}

void Calculator::clearError()
{
    if (m_hasError || !m_errorMessage.isEmpty()) {
        m_hasError = false;
        m_errorMessage.clear();
        emit errorChanged();
    }
}

void Calculator::checkForVariable()
{
    const bool hadVariable = m_hasVariable;
    m_hasVariable = FunctionEvaluator::containsVariable(m_expression);
    if (hadVariable != m_hasVariable) {
        emit hasVariableChanged();
    }
}

QString Calculator::prepareExpression(const QString &raw)
{
    QString exp = raw.trimmed();
    while (!exp.isEmpty() && trailingOps.match(exp.right(1)).hasMatch())
        exp.chop(1);

    int balance = 0;
    for (QChar ch : exp) { if (ch== '(') ++balance; else if (ch==')') --balance; }
    while (balance-- > 0) exp.append(')');

    return exp;
}

void Calculator::appendDigit(const QString &d)
{
    clearError();
    if (m_lastWasResult) {
        clear();
        m_lastWasResult = false;
    }

    setCurrentNumber(m_currentNumber == "0" ? d : m_currentNumber+d);
    setExpression(m_expression+d);
    evaluatePreview();
}

void Calculator::appendOperator(const QString &op)
{
    clearError();
    if (m_lastWasResult) {
        setExpression(m_currentNumber);
        m_lastWasResult=false;
    }

    if (op != "!" && !m_expression.isEmpty() &&
        trailingOps.match(m_expression.right(1)).hasMatch())
        setExpression(m_expression.left(m_expression.length()-1)+op);
    else
        setExpression(m_expression+op);

    if (op != "!") setCurrentNumber("");
    evaluatePreview();
}

void Calculator::appendDot()
{
    clearError();
    if (!m_currentNumber.contains('.')) {
        setCurrentNumber(m_currentNumber.isEmpty() ? "0." : m_currentNumber+'.');
        setExpression(m_expression+'.');
        evaluatePreview();
    }
}

void Calculator::appendVariable()
{
    clearError();
    if (m_lastWasResult) {
        clear();
        m_lastWasResult = false;
    }

    m_currentNumber.clear();
    setExpression(m_expression + "x");
    evaluatePreview();
}

void Calculator::deleteLast()
{
    clearError();

    if (m_expression.isEmpty())
        return;

    const long long len = m_expression.length();
    if (m_expression.back() == '(') {
        long long i = len - 2;
        while (i >= 0 && m_expression[i].isLetter())
            --i;

        const long long funcStart = i + 1;
        const QString func = m_expression.mid(funcStart, len - funcStart - 1);

        static const QSet<QString> funcs = { "ln", "sin", "cos", "tan", "log", "sqrt", "abs", "arccos", "arcsin", "arctan" };
        if (funcs.contains(func))
            setExpression(m_expression.left(funcStart));
        else
            setExpression(m_expression.left(len - 1));

        if (m_openParensCount > 0)
            --m_openParensCount;
    }
    else {
        setExpression(m_expression.left(len - 1));
        if (!m_currentNumber.isEmpty())
            setCurrentNumber(m_currentNumber.left(m_currentNumber.length() - 1));
    }

    evaluatePreview();
}

void Calculator::clear()
{
    setCurrentNumber("");
    setExpression("");
    m_previewResult.clear();
    emit previewResultChanged();
    m_openParensCount = 0;
    clearError();
}

void Calculator::evaluatePreview()
{
    const QString prep = prepareExpression(m_expression);
    if (prep.isEmpty()) {
        if (!m_previewResult.isEmpty()) {
            m_previewResult = ""; emit previewResultChanged();
        }
        return;
    }

    if (m_hasVariable) {
        if (!m_previewResult.isEmpty()) {
            m_previewResult = ""; emit previewResultChanged();
        }
        return;
    }

    bool ok = false;
    const double res = ArithmeticEvaluator::evaluate(prep, &ok);
    if (const QString s = formatResult(res); ok && s != "nan") {
        if (s != m_previewResult) {
            m_previewResult = s; emit previewResultChanged();
        }
    } else if (!m_previewResult.isEmpty()) {
        m_previewResult = ""; emit previewResultChanged();
    }
}

void Calculator::evaluateResult()
{
    clearError();
    const QString prep=prepareExpression(m_expression);

    if (m_hasVariable) {
        m_hasError = true;
        m_errorMessage = "Выражение является функцией";
        emit errorChanged();
        return;
    }

    bool ok = false;
    const double res = ArithmeticEvaluator::evaluate(prep, &ok);

    if (const QString s = formatResult(res); ok && s != "nan" && s != "inf") {
        setCurrentNumber(s); setExpression(s); m_lastWasResult = true;
        if (!m_previewResult.isEmpty()) {
            m_previewResult = ""; emit previewResultChanged();
        }
        emit calculationPerformed(prep, s);
    } else {
        m_hasError = true;
        m_errorMessage = "Недопустимое выражение";
        emit errorChanged();
    }
}

QString Calculator::formatResult(const double res) {
    if (qAbs(res - qRound64(res)) < 1e-12)
        return QString::number(qRound64(res));

    return QString::number(res, 'g', 17);
}

void Calculator::addDecimalPoint() { appendDot(); }
void Calculator::backspace() { deleteLast(); }
void Calculator::calculate() { evaluateResult(); }
void Calculator::sqrt() { appendFunction("sqrt"); }
void Calculator::power() { appendOperator("^"); }
void Calculator::factorial() { appendOperator("!"); }

void Calculator::appendFunction(const QString &func)
{
    clearError();
    if (m_lastWasResult) {
        clear();
        m_lastWasResult = false;
    }
    setExpression(m_expression + func + '(');
    ++m_openParensCount;
    setCurrentNumber("");
    evaluatePreview();
}

void Calculator::appendConstant(const QString &symbol)
{
    clearError();
    if (m_lastWasResult) {
        clear();
        m_lastWasResult = false;
    }

    m_currentNumber.clear();
    setExpression(m_expression + symbol);
    evaluatePreview();
}

void Calculator::addParenthesis()
{
    clearError();
    const bool close = m_openParensCount>0 && !m_expression.isEmpty() &&
                       (m_expression.back().isDigit() ||
                           m_expression.back()==')' ||
                           m_expression.back() == '!' ||
                           m_expression.back()=='e' ||
                           m_expression.back()=='x' ||
                           m_expression.back().unicode() == 0x03C0); // π
    if (close) {
        setExpression(m_expression+')'); --m_openParensCount;
    } else {
        if (m_lastWasResult) {
            clear();
            m_lastWasResult = false;
        }
        setExpression(m_expression+'(');
        ++m_openParensCount;
    }
    evaluatePreview();
}

void Calculator::percentage()
{
    clearError();
    if (m_currentNumber.isEmpty()) return;

    bool ok = false;
    double v = m_currentNumber.toDouble(&ok);
    if (!ok) {
        m_hasError=true; m_errorMessage="Ошибка преобразования числа";
        emit errorChanged();
        return;
    }

    v/=100.0;
    const QString s=QString::number(v,'g',15);
    if (m_expression.endsWith(m_currentNumber))
        setExpression(m_expression.left(m_expression.length() - m_currentNumber.length()) + s);
    else
        setExpression(m_expression+s);
    setCurrentNumber(s);
    evaluatePreview();
}

void Calculator::loadExpression(const QString &expr)
{
    clearError();
    setExpression(expr);
    setCurrentNumber("");
    m_lastWasResult = false;
    evaluatePreview();
}

void Calculator::toggleAngleMode()
{
    m_angleDegrees = !m_angleDegrees;
    Parser::setAngleMode(m_angleDegrees);
    emit angleModeChanged();
    evaluatePreview();
}

void Calculator::requestPlot()
{
    if (m_hasVariable && !m_expression.isEmpty()) {
        const QString prep = prepareExpression(m_expression);
        if (!m_previewResult.isEmpty()) {
            m_previewResult = ""; emit previewResultChanged();
        }
        emit plotRequested(prep);
        emit calculationPerformed(prep, "Функция");
    }
}
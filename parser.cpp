#include "parser.h"
#include <QRegularExpression>
#include <QtMath>
#include <QDebug>

double Parser::evaluate(const QString &expression, bool *ok)
{
    if (expression.isEmpty()) {
        if (ok) *ok = false;
        return 0.0;
    }

    try {
        const QList<Token> tokens = tokenize(expression);
        const QList<Token> postfix = infixToPostfix(tokens);

        if (ok) *ok = true;
        return evaluatePostfix(postfix);
    } catch (const std::exception &e) {
        qWarning() << "Ошибка парсера:" << e.what();
        if (ok) *ok = false;
        return 0.0;
    }
}

QList<Parser::Token> Parser::tokenize(const QString &expression) {
    QList<Token> tokens;

    QString cleanExpr = expression;
    cleanExpr.remove(' ');

    int i = 0;
    while (i < cleanExpr.length()) {
        QChar c = cleanExpr[i];

        if (c == '-' &&
            (i == 0 || cleanExpr[i-1] == '(' || cleanExpr[i-1] == '+' || cleanExpr[i-1] == '-' ||
             cleanExpr[i-1] == '*' || cleanExpr[i-1] == '/' || cleanExpr[i-1] == '^' || cleanExpr[i-1] == '%') &&
            (i + 1 < cleanExpr.length()))
        {
            QChar next = cleanExpr[i + 1];

            if (next.isDigit() || next == '.') {
                QString number = "-";
                ++i;

                while (i < cleanExpr.length() && (cleanExpr[i].isDigit() || cleanExpr[i] == '.'))
                    number += cleanExpr[i++];

                if (i < cleanExpr.length() && (cleanExpr[i] == 'e' || cleanExpr[i] == 'E')) {
                    number += cleanExpr[i++];

                    if (i < cleanExpr.length() && (cleanExpr[i] == '+' || cleanExpr[i] == '-'))
                        number += cleanExpr[i++];

                    bool expDigits = false;
                    while (i < cleanExpr.length() && cleanExpr[i].isDigit()) {
                        expDigits = true;
                        number += cleanExpr[i++];
                    }

                    if (!expDigits)
                        throw std::invalid_argument("Неверная экспонента в числе: " + number.toStdString());
                }

                bool okNum = false;
                const double numVal = number.toDouble(&okNum);

                if (!okNum)
                    throw std::invalid_argument("Неверное число: " + number.toStdString());

                tokens.append(Token{Token::Number, number, numVal, 0, false});
                continue;
            }

            if (next == '(' || next.isLetter()) {
                tokens.append(Token{Token::Number, "-1", -1.0, 0, false});

                Token tOp{Token::Operator, "*", 0.0, getOperatorPrecedence("*"), isRightAssociative("*")};
                tokens.append(tOp);

                ++i;
                continue;
            }
        }

        switch (c.unicode()) {
        case '(': {
            tokens.append(Token{Token::LeftParen, "(", 0.0, 0, false});
            ++i;
            break;
        }
        case ')': {
            tokens.append(Token{Token::RightParen, ")", 0.0, 0, false});
            ++i;
            break;
        }
        case '+': case '-': case '*': case '/': case '^': case '%': {
            Token t{Token::Operator, QString(c), 0.0, getOperatorPrecedence(QString(c)), isRightAssociative(QString(c))};
            tokens.append(t);
            ++i;
            break;
        }
        case '!': {
            if (i == 0 || !(cleanExpr[i-1].isDigit() || cleanExpr[i-1] == ')'))
                throw std::invalid_argument("Недостаточно операндов для факториала");

            Token t{Token::Operator, "!", 0.0, getOperatorPrecedence("!"), isRightAssociative("!")};
            tokens.append(t);
            ++i;
            break;
        }
        case 0x03C0: { // π
            tokens.append(Token{Token::Number, "π", 3.14159265358979323846, 0, false});
            ++i;
            continue;
        }
        case 'e': {
            tokens.append(Token{Token::Number, "e", exp(1), 0, false});
            ++i;
            continue;
        }
        default: {
            if (c.isDigit() || c == '.') {
                QString number;
                while (i < cleanExpr.length() && (cleanExpr[i].isDigit() || cleanExpr[i] == '.'))
                    number += cleanExpr[i++];

                if (i < cleanExpr.length() && (cleanExpr[i] == 'e')) {
                    number += cleanExpr[i++];

                    if (i < cleanExpr.length() && (cleanExpr[i] == '+' || cleanExpr[i] == '-'))
                        number += cleanExpr[i++];

                    bool expDigits = false;
                    while (i < cleanExpr.length() && cleanExpr[i].isDigit()) {
                        expDigits = true;
                        number += cleanExpr[i++];
                    }

                    if (!expDigits)
                        throw std::invalid_argument("Неверная экспонента в числе: " + number.toStdString());
                }

                bool okNum = false;
                double numVal = number.toDouble(&okNum);

                if (!okNum)
                    throw std::invalid_argument("Неверное число: " + number.toStdString());

                tokens.append(Token{Token::Number, number, numVal, 0, false});
            } else if (c.isLetter()) {
                QString func;
                const int funcStart = i;

                while (i < cleanExpr.length() && cleanExpr[i].isLetter())
                    func += cleanExpr[i++];

                if (isFunction(func)) {
                    if (funcStart > 0) {
                        if (QChar prev = cleanExpr[funcStart - 1]; prev.isDigit() || prev == ')' || prev == '.')
                            throw std::invalid_argument("Нужен оператор перед функцией «" + func.toStdString() + "»");
                    }

                    tokens.append(Token{Token::Function, func, 0.0, 4, false});
                } else {
                    throw std::invalid_argument("Неизвестный символ в выражении: " + func.toStdString());
                }
            } else {
                throw std::invalid_argument("Неизвестный символ в выражении: " + QString(c).toStdString());
            }
        }
        }
    }

    return tokens;
}

QList<Parser::Token> Parser::infixToPostfix(const QList<Token> &tokens)
{
    QList<Token> output;
    QStack<Token> operators;

    for (const Token &token : tokens) {
        switch (token.type) {
        case Token::Number:
            output.append(token);
            break;

        case Token::Function:
            operators.push(token);
            break;

        case Token::Operator:
            while (!operators.isEmpty() &&
                   operators.top().type != Token::LeftParen &&
                   (operators.top().type == Token::Function ||
                    operators.top().precedence > token.precedence ||
                    (operators.top().precedence == token.precedence && !token.rightAssociative))) {
                output.append(operators.pop());
            }
            operators.push(token);
            break;

        case Token::LeftParen:
            operators.push(token);
            break;

        case Token::RightParen:
            while (!operators.isEmpty() && operators.top().type != Token::LeftParen) {
                output.append(operators.pop());
            }

            if (!operators.isEmpty() && operators.top().type == Token::LeftParen)
                operators.pop();

            if (!operators.isEmpty() && operators.top().type == Token::Function)
                output.append(operators.pop());
            break;
        }
    }

    while (!operators.isEmpty()) {
        if (operators.top().type == Token::LeftParen)
            throw std::invalid_argument("Неверное количество скобок");
        output.append(operators.pop());
    }

    return output;
}

double Parser::evaluatePostfix(const QList<Token> &tokens)
{
    QStack<double> stack;

    for (const Token &token : tokens) {
        switch (token.type) {
        case Token::Number:
            stack.push(token.numValue);
            break;

        case Token::Operator:
            if (token.value == "!") {
                if (stack.isEmpty())
                    throw std::invalid_argument("Недостаточно операндов для '!'");

                stack.push(factorial(stack.pop()));
            } else {
                if (stack.size() < 2)
                    throw std::invalid_argument("Недостаточно операндов для оператора " + token.value.toStdString());

                const double right = stack.pop();
                const double left = stack.pop();
                stack.push(applyOperator(token.value, left, right));
            }
            break;

        case Token::Function:
            if (stack.isEmpty())
                throw std::invalid_argument("Недостаточно аргументов для функции " + token.value.toStdString());
            stack.push(applyFunction(token.value, stack.pop()));
            break;

        default:
            throw std::invalid_argument("Неожиданный токен в постфикс-выражении");
        }
    }

    if (stack.size() != 1)
        throw std::invalid_argument("Неверное выражение");

    return stack.pop();
}

int Parser::getOperatorPrecedence(const QString &op)
{
    switch (op[0].unicode()) {
    case '+': case '-': return 1;
    case '*': case '/': case '%': return 2;
    case '^': return 3;
    case '!': return 4;
    default: return 0;
    }
}

bool Parser::isRightAssociative(const QString &op)
{
    switch (op[0].unicode()) {
    case '^': return true;
    default: return false;
    }
}

bool Parser::isFunction(const QString &str)
{
    static const QStringList functions = {
        "sin", "cos", "tan", "ln", "log", "sqrt", "abs", "arcsin", "arccos", "arctan"
    };
    return functions.contains(str);
}

double Parser::applyOperator(const QString &op, const double left, const double right)
{
    switch (op[0].unicode()) {
    case '+': return left + right;
    case '-': return left - right;
    case '*': return left * right;
    case '/':
        if (qFuzzyIsNull(right)) throw std::runtime_error("Деление на ноль");
        return left / right;
    case '%':
        if (qFuzzyIsNull(right)) throw std::runtime_error("Деление на ноль в операции остатка");
        return fmod(left, right);
    case '^': return qPow(left, right);
    default: throw std::invalid_argument("Неизвестный оператор: " + op.toStdString());
    }
}

double Parser::applyFunction(const QString &func, const double arg)
{
    if (func == "sin") return qSin(isDegrees()? qDegreesToRadians(arg) : arg);
    if (func == "cos") return qCos(isDegrees()? qDegreesToRadians(arg) : arg);
    if (func == "tan") return qTan(isDegrees()? qDegreesToRadians(arg) : arg);
    if (func == "ln") { if (arg <= 0) throw std::runtime_error("ln(x≤0)"); return qLn(arg); }
    if (func == "log") { if (arg <= 0) throw std::runtime_error("log(x≤0)"); return log10(arg); }
    if (func == "sqrt") { if (arg < 0) throw std::runtime_error("√(x<0)"); return qSqrt(arg); }
    if (func == "abs") return qFabs(arg);
    if (func == "arctan") {
        const double res = qAtan(arg);
        return isDegrees() ? qRadiansToDegrees(res) : res;
    }
    if (func == "arcsin") {
        if (arg < -1 || arg > 1) throw std::runtime_error("arcsin(x<-1 || x>1)");
        const double res = qAsin(arg);
        return isDegrees() ? qRadiansToDegrees(res) : res;
    }
    if (func == "arccos") {
        if (arg < -1 || arg > 1) throw std::runtime_error("arccos(x<-1 || x>1)");
        const double res = qAcos(arg);
        return isDegrees() ? qRadiansToDegrees(res) : res;
    }

    throw std::invalid_argument("Неизвестная функция: " + func.toStdString());
}

double Parser::factorial(const double n)
{
    if (n < 0 || n != qFloor(n))
        throw std::runtime_error("Факториал определён только для неотрицательных целых чисел");

    if (n == 0 || n == 1) return 1;

    double res = 1;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

bool Parser::s_degrees = false;
void Parser::setAngleMode(const bool degrees) { s_degrees = degrees; }
bool Parser::isDegrees()                { return s_degrees; }

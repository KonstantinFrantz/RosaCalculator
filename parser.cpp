#include "parser.h"
#include <QRegularExpression>
#include <QtMath>
#include <QDebug>

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
            if (i == 0 || (!cleanExpr[i-1].isDigit() && cleanExpr[i-1] != ')' && cleanExpr[i-1] != 'x'))
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

                if (func == "x") {
                    tokens.append(Token{Token::Variable, "x", 0.0, 0, false});
                }

                else if (isFunction(func)) {
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

        case Token::Variable:
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

bool Parser::s_degrees = false;
void Parser::setAngleMode(const bool degrees) { s_degrees = degrees; }
bool Parser::isDegrees() { return s_degrees; }

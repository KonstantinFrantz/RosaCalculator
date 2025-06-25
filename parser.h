#ifndef PARSER_H
#define PARSER_H

#include <QStack>
#include <QStringList>

class Parser
{
public:
    struct Token {
        enum Type {
            Number,
            Operator,
            Function,
            LeftParen,
            RightParen,
            Variable
        };

        Type type;
        QString value;
        double numValue;
        int precedence;
        bool rightAssociative;
    };

    static QList<Token> tokenize(const QString &expression);
    static QList<Token> infixToPostfix(const QList<Token> &tokens);
    static int getOperatorPrecedence(const QString &op);
    static bool isRightAssociative(const QString &op);
    static bool isFunction(const QString &str);
    static void setAngleMode(bool degrees);
    static bool isDegrees();

private:
    static bool s_degrees;
};

#endif // PARSER_H

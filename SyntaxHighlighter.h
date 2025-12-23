//
// Created by peterglenn on 12/21/25.
//

#ifndef EXPLORE_SYNTAXHIGHLIGHTER_H
#define EXPLORE_SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QList>
#include <QPlainTextEdit>

struct ParenthesisInfo {
    QChar character;
    int position;
};
struct BlockData : public QTextBlockUserData {
    QList<ParenthesisInfo> parentheses;
};

class MyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MyHighlighter(QPlainTextEdit* p);

protected:
    QPlainTextEdit* textEdit;
    QTextCharFormat baseformat;
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> highlightingRules;
};


#endif //EXPLORE_SYNTAXHIGHLIGHTER_H
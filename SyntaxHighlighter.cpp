//
// Created by peterglenn on 12/21/25.
//

#include "SyntaxHighlighter.h"


MyHighlighter::MyHighlighter(QPlainTextEdit *p)
    : QSyntaxHighlighter(p->document()), textEdit(p)
{
    HighlightingRule rule;

    // Keyword format (example)
    QTextCharFormat keywordFormat;
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setForeground(Qt::darkBlue);
    QStringList keywordPatterns = {"\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bdouble\\b"};
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    baseformat = p->currentCharFormat();

    // Add other rules (e.g., comments, strings, etc.)
    // ...



}

void MyHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    // For multi-line highlighting (like C++ comments), you'd manage the block's user state.


    if (textEdit->textCursor().block().userData()) {
        setFormat(0,textEdit->document()->toPlainText().length(),baseformat);
        auto pi = static_cast<BlockData*>(textEdit->textCursor().block().userData());
        auto pos = textEdit->textCursor().position();
        QTextCharFormat format;
        format.setForeground(Qt::darkBlue);
        format.setBackground(Qt::yellow);


        if (pi->parentheses.size() > 0) {
            if (pos >= pi->parentheses.size())
                pos = pi->parentheses.size()-1;
            auto a = pi->parentheses[pos].position;
            if (a >= 0) {
                if (a < pos)
                    setFormat(a, pos-a+1, format );
                else
                    setFormat(pos, a-pos+1, format );
            }
            if (pos > 0) {
                auto b = pi->parentheses[pos-1].position;
                if (b >= 0) {
                    if (b < pos)
                        setFormat(b, pos-b, format );
                    else
                        setFormat(pos, b-pos, format );
                }
            }
        }
    }
}


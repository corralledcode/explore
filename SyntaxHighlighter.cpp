//
// Created by peterglenn on 12/21/25.
//

#include "SyntaxHighlighter.h"

#include "config.h"


MyHighlighter::MyHighlighter(QPlainTextEdit *p)
    : QSyntaxHighlighter(p->document()), textEdit(p)
{

    QTextCharFormat keywordFormat;
    // keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setForeground(Qt::darkGreen);

    QStringList keywordList;
    // Keyword format (example)
    HighlightingRule rule;
    QStringList keywordPatterns {};
    foreach (const auto &pair, stringoperatorsmap) {
        std::string keyword = "\\b" + pair.first + "\\b";
        keywordPatterns.append( QtPrivate::convertToQString(keyword));
    }
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    keywordPatterns = {};
    foreach (const auto &measurestring, measures) {
        std::string keyword = "\\b" + measurestring + "\\b";
        keywordPatterns.append( QtPrivate::convertToQString(keyword));
    }

    keywordFormat.setForeground(Qt::magenta);
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
        // setFormat(0,textEdit->document()->toPlainText().length(),baseformat);
        auto pi = static_cast<BlockData*>(textEdit->textCursor().block().userData());
        auto cursor = textEdit->textCursor();
        auto pos = cursor.position();
        QTextCharFormat endformat;
        endformat.setForeground(Qt::darkBlue);
        endformat.setBackground(Qt::yellow);
        QTextCharFormat format;
        format.setForeground(Qt::white);
        format.setBackground(Qt::darkGray);

        if (pi->parentheses.size() > 0) {
            if (pos >= pi->parentheses.size())
                pos = pi->parentheses.size()-1;
            auto a = pi->parentheses[pos].position;
            if (a >= 0) {
                if (a < pos) {
                    setFormat(a+1, pos-a, format );
                    setFormat( a, 1, endformat);
                    setFormat( pos, 1, endformat);
                } else {
                    setFormat(pos+1, a-pos, format );
                    setFormat( pos, 1, endformat);
                    setFormat( a, 1, endformat);
                }
            }
            /*
            if (pos > 0) {
                auto b = pi->parentheses[pos-1].position;
                if (b >= 0) {
                    if (b < pos) {
                        setFormat(b+1, pos-b - 1, format );
                        setFormat( pos - 1, 1, endformat);
                        setFormat( b, 1, endformat);
                    } else {
                        setFormat(pos-1, b-pos, format );
                        setFormat( b, 1, endformat);
                        setFormat( pos, 1, endformat);
                    }
                }
            }*/
        }
    }
}


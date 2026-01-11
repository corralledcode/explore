//
// Created by peterglenn on 12/21/25.
//

#include "SyntaxHighlighter.h"

#include <iostream>
#include <bits/fs_fwd.h>

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

        auto pos = cursor.position() - 1;
        if (pos < 0)
            pos = 0;

        // std::cout << pos << ": " << text.toStdString() << std::endl;
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
                auto b = currentBlock();
                if (b.position() <= pos && pos < (b.position() + b.length()))
                    setFormat( pos - b.position(), 1, endformat);
                if (b.position() <= a && a < (b.position() + b.length()))
                    setFormat( a - b.position(), 1, endformat);
            } else
                if (++pos < pi->parentheses.size()) {
                    a = pi->parentheses[pos].position;
                    if (a >= 0) {
                        auto b = currentBlock();
                        if (b.position() <= pos && pos < (b.position() + b.length()))
                            setFormat( pos - b.position(), 1, endformat);
                        if (b.position() <= a && a < (b.position() + b.length()))
                            setFormat( a - b.position(), 1, endformat);
                    }
                }
        }
    }
}


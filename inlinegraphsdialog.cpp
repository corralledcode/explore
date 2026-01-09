#include "inlinegraphsdialog.h"

#include <QPlainTextEdit>

#include "./ui_inlinegraphsdialog.h"

InlineGraphsDialog::InlineGraphsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InlineGraphsDialog)
{
    ui->setupUi(this);
    connect(findChild<QToolButton*>("addbutton"), &QToolButton::clicked, this, &InlineGraphsDialog::onAddButtonClicked);
    connect(findChild<QToolButton*>("removebutton"), &QToolButton::clicked, this, &InlineGraphsDialog::onRemoveButtonClicked);
    connect(findChild<QToolButton*>("duplicatebutton"), &QToolButton::clicked, this, &InlineGraphsDialog::onDuplicateButtonClicked);
}

InlineGraphsDialog::~InlineGraphsDialog()
{
    delete ui;
}

void InlineGraphsDialog::verticallyalign () {
    auto igW = findChild<QWidget*>("inlinegraphswidget");
    if (!igW->layout())
        new QVBoxLayout(igW);
    else {
        for (auto o : igW->layout()->children())
            igW->layout()->removeWidget(dynamic_cast<QWidget *>(o));
    }
    for (auto o : igW->children()) {
        if (auto q = dynamic_cast<QPlainTextEdit*>(o))
            igW->layout()->addWidget(q);
    }
}

void InlineGraphsDialog::add( std::string s ) {
    auto igW = findChild<QWidget*>("inlinegraphswidget");
    auto textedit = new QPlainTextEdit(igW);
    textedit->setPlainText(s.c_str());
    textedit->setMinimumHeight(40);
    verticallyalign();
}

void InlineGraphsDialog::remove( std::string s ) {
    auto igW = findChild<QWidget*>("inlinegraphswidget");
    int i = 0;
    while (i < igW->children().size() ) {
        if (auto te = dynamic_cast<QPlainTextEdit*>(igW->children().at(i))) {
            if (te->toPlainText() == s) {
                delete te;
                break;
            }
        }
        ++i;
    }
    verticallyalign();
}

void InlineGraphsDialog::onAddButtonClicked() {
    add("");
};

void InlineGraphsDialog::onRemoveButtonClicked() {
    auto igW = findChild<QWidget*>("inlinegraphswidget");
    auto w = igW->focusWidget();
    if ( auto te = dynamic_cast<QPlainTextEdit*>(w) )
        remove(te->toPlainText().toStdString());
}

void InlineGraphsDialog::onDuplicateButtonClicked() {
    auto igW = findChild<QWidget*>("inlinegraphswidget");
    auto w = igW->focusWidget();
    if ( auto te = dynamic_cast<QPlainTextEdit*>(w) )
        add(te->toPlainText().toStdString());
}

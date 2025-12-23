#include "verbositydialog.h"
#include "ui_verbositydialog.h"

VerbosityDialog::VerbosityDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VerbosityDialog)
{
    ui->setupUi(this);
}

VerbosityDialog::~VerbosityDialog()
{
    delete ui;
}

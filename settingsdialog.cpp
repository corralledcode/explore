#include "settingsdialog.h"

#include <QFileDialog>
#include <QPushButton>

#include "config.h"
#include "ui_settingsdialog.h"

settingsDialog::settingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingsDialog)
{
    ui->setupUi(this);
    connect(ui->flagcalcexecutablebutton, &QPushButton::clicked, this, &settingsDialog::onflagcalcexecutablebuttonClicked);
    connect(ui->defaultdirectorybutton, &QPushButton::clicked, this, &settingsDialog::ondefaultdirectorybuttonClicked);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &settingsDialog::onokbuttonClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &settingsDialog::oncancelbuttonClicked);

    ui->flagcalcexecutablepathedit->setText(FLAGCALCEXECUTABLEPATH.c_str());
    ui->flagcalcexecutablenameedit->setText(FLAGCALCEXECUTABLENAME.c_str());
    ui->flagcalcheaderedit->setText(FLAGCALCHEADER.c_str());
    ui->flagcalcinvokeedit->setText(FLAGCALCINVOKE.c_str());
    ui->defaultdirectoryedit->setText(DEFAULTDIRECTORY.c_str());

}


settingsDialog::~settingsDialog()
{
    delete ui;
}

void settingsDialog::onflagcalcexecutablebuttonClicked() {
    openflagcalcexecutablefileDialog();
}

void settingsDialog::ondefaultdirectorybuttonClicked() {
    opendefaultdirectoryDialog();
}

int settingsDialog::openflagcalcexecutablefileDialog() {

    // The static function returns a QString with the selected file path
    QString directoryName = QFileDialog::getExistingDirectory(
        this,                                        // Parent widget (e.g., your QMainWindow or QWidget)
        tr("Choose Flagcalc path"),                             // Dialog title
        FLAGCALCEXECUTABLEPATH.c_str()                      // Default directory (e.g., a specific folder)
    );

    // Check if a file was selected (fileName will be empty if the user cancelled)
    if (!directoryName.isEmpty()) {
        ui->flagcalcexecutablepathedit->setText(directoryName);
        return 0;
    }
    return 1;
}

int settingsDialog::opendefaultdirectoryDialog() {

    // The static function returns a QString with the selected file path
    QString directoryName = QFileDialog::getExistingDirectory(
        this,                                        // Parent widget (e.g., your QMainWindow or QWidget)
        tr("Choose default directory"),                             // Dialog title
        DEFAULTDIRECTORY.c_str()
    );

    // Check if a file was selected (fileName will be empty if the user cancelled)
    if (!directoryName.isEmpty()) {
        ui->defaultdirectoryedit->setText(directoryName);
        return 0;
    }
    return 1;
}

void settingsDialog::onokbuttonClicked() {
    FLAGCALCEXECUTABLEPATH = ui->flagcalcexecutablepathedit->text().toStdString();
    FLAGCALCEXECUTABLENAME = ui->flagcalcexecutablenameedit->text().toStdString();
    FLAGCALCHEADER = ui->flagcalcheaderedit->text().toStdString();
    FLAGCALCINVOKE = ui->flagcalcinvokeedit->text().toStdString();
    DEFAULTDIRECTORY = ui->defaultdirectoryedit->text().toStdString();
    close();
}

void settingsDialog::oncancelbuttonClicked() {
    close();
}
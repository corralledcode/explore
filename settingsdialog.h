#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class settingsDialog;
}

class settingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDialog(QWidget *parent = nullptr);
    ~settingsDialog();

    int openflagcalcexecutablefileDialog();
    int opendefaultdirectoryDialog();

private:
    Ui::settingsDialog *ui;


private slots:

    void onflagcalcexecutablebuttonClicked();
    void ondefaultdirectorybuttonClicked();
    void onokbuttonClicked();
    void oncancelbuttonClicked();
};

#endif // SETTINGSDIALOG_H

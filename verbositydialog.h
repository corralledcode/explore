#ifndef VERBOSITYDIALOG_H
#define VERBOSITYDIALOG_H

#include <QDialog>

namespace Ui {
class VerbosityDialog;
}

class VerbosityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VerbosityDialog(QWidget *parent = nullptr);
    ~VerbosityDialog();

private:
    Ui::VerbosityDialog *ui;
};

#endif // VERBOSITYDIALOG_H

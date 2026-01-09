#ifndef INLINEGRAPHSDIALOG_H
#define INLINEGRAPHSDIALOG_H

#include <QDialog>

namespace Ui {
class InlineGraphsDialog;
}

class InlineGraphsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InlineGraphsDialog(QWidget *parent = nullptr);
    ~InlineGraphsDialog();
    void verticallyalign();
    void add(std::string);
    void remove(std::string);

private:
    Ui::InlineGraphsDialog *ui;


private slots:

    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onDuplicateButtonClicked();

};

#endif // INLINEGRAPHSDIALOG_H

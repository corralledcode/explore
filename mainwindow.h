#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "fcinstance.h"
#include "inlinegraphsdialog.h"
#include "SyntaxHighlighter.h"
#include "verbositydialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QPlainTextEdit* currentqueryedit;
    QPlainTextEdit* currentcr1edit;
    QPlainTextEdit* currentcr2edit;
    QPlainTextEdit* currentcr3edit;

    void addNewQueryTab(const bool duplicate = false);
    void deleteQueryTab();
    std::vector<fcinstanceQtbridge*> fcbridges {};
    std::string outputfilename {};
    void runQuery();
    void logQuery();
    void runandlogQuery();
    MainWindow(fcinstance fcin, QWidget *parent = nullptr);
    ~MainWindow();
    int currentfcindex = 0;
    std::string logfilename {};

    int openQueryFileDialog();
    VerbosityDialog *verbosityDialog {};
    InlineGraphsDialog *inlinegraphsDialog {};

    void opensettingsDialog();

private slots:

    void onRunQueryButtonClicked();
    void onDeleteQueryButtonClicked();
    void onNewQueryButtonClicked();
    void onDuplicateQueryButtonClicked();

    void onrandomizeredgecountvaluechanged(const int& value);
    void onrandomizeredgecountcombochanged(const QString& text);
    void onautocheckrandomizer();
    void onautocheckgraphfiles();
    void onautocheckinlinegraphs();

    void onoutchanged();
    void onoutfilecheckboxchanged(bool clicked);

    void onquerycriteditcursorpositionchanged();
    void onquerycritedittextchanged();

    void onAddToolButtonClicked();
    void onRemoveToolButtonClicked();
    void onTreeSelectionChanged();
    void onTreeDoubleClicked(const QModelIndex &index);

    void ongraphfilesbuttonclicked();

    void onQueryTabCurrentChanged(int index);

    void onlogfilecombocurrenttextchanged();
    void onstoredproceduresbuttonClicked();

    void onpythonbuttonClicked();
    void onlogfilebuttonClicked();
    void onoutfilebuttonClicked();
    void oninlinegraphsbuttonclicked();
    void onrunandlogbuttonClicked();
    void onLogQueryButtonClicked();
    void onlogfileopenbuttonClicked();
    void onlogfileclearbuttonClicked();
    void onverbositybuttonClicked();
    void onoutputfilebuttonClicked();
    void onclearoutputbuttonClicked();
    void onoutputopenbuttonClicked();
    void onclearoutputfilebuttonClicked();
    void onrunbuttonClicked();
    void onoutputfilecomboboxchanged();
    void onoutfileopenbuttonClicked();

    void onmovequerybuttonClicked();

    void handleverbosityDialogClose(int);
    void handleinlinegraphsDialogClose(int);

    void onsettingsbuttonClicked();


private:
    Ui::MainWindow *ui;
    bool querycritedittextchangedmutex = false;
};
#endif // MAINWINDOW_H

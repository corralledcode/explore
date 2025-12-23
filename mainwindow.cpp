#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "verbositydialog.h"
#include "./ui_verbositydialog.h"
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <string>
#include <QProcess>

#include "querytab.h"
#include "SyntaxHighlighter.h"
#include "config.h"

void logsyntaxhighlightchange( QPlainTextEdit* textedit );


MainWindow::MainWindow(fcinstance fcin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ... set geometry or layout for the button ...

    // Connect the button's 'clicked()' signal to your 'onButtonClicked()' slot


    // connect(ui->logfilebutton, &QPushButton::clicked, this, &MainWindow::onlogfilebuttonClicked);

    connect(ui->logfilenamebutton, &QPushButton::clicked, this, &MainWindow::onlogfilebuttonClicked);

    connect(ui->logfileopenbutton, &QPushButton::clicked, this, &MainWindow::onlogfileopenbuttonClicked);

    connect(ui->LogandRunQueryButton, &QPushButton::clicked, this, &MainWindow::onrunandlogbuttonClicked);
    connect(ui->LogQueryButton, &QPushButton::clicked, this, &MainWindow::onLogQueryButtonClicked);
    connect(ui->RunQueryButton, &QPushButton::clicked, this, &MainWindow::onRunQueryButtonClicked);
     // connect(ui->logfileclearbutton, &QPushButton::clicked, this, &MainWindow::onlogfileclearbuttonClicked);

    connect(ui->outputfilebutton, &QPushButton::clicked, this, &MainWindow::onoutputfilebuttonClicked);
    connect(ui->outputopenbutton, &QPushButton::clicked, this, &MainWindow::onoutputopenbuttonClicked);
    connect(ui->clearoutputfilebutton, &QPushButton::clicked, this, &MainWindow::onclearoutputfilebuttonClicked);
    connect(ui->clearoutputbutton, &QPushButton::clicked, this, &MainWindow::onclearoutputbuttonClicked);


    connect(ui->NewQueryButton, &QPushButton::clicked, this, &MainWindow::onNewQueryButtonClicked);
    connect(ui->DeleteQueryButton, &QPushButton::clicked, this, &MainWindow::onDeleteQueryButtonClicked);
    connect(ui->AddToolButton, &QPushButton::clicked, this, &MainWindow::onAddToolButtonClicked);
    connect(ui->RemoveToolButton, &QPushButton::clicked, this, &MainWindow::onRemoveToolButtonClicked);
    connect(ui->movequerybutton, &QPushButton::clicked, this, &MainWindow::onmovequerybuttonClicked);
    connect(ui->scripttree, &QTreeWidget::itemSelectionChanged, this, &MainWindow::onTreeSelectionChanged);
    connect(ui->scripttree, &QTreeWidget::doubleClicked, this, &MainWindow::onTreeDoubleClicked);

    connect(ui->logfilecombo, &QComboBox::currentTextChanged, this, &MainWindow::onlogfilecombocurrenttextchanged);



    // Add an initial tab

    fcbridges.push_back(new fcinstanceQtbridge(ui, nullptr));

    addNewQueryTab();
    ui->querytabs->setCurrentIndex(0);
    deleteQueryTab();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRunQueryButtonClicked() {
    runQuery();
}

void MainWindow::onLogQueryButtonClicked() {
    logQuery();
}

void MainWindow::onNewQueryButtonClicked() {
    addNewQueryTab();
}

void MainWindow::onDeleteQueryButtonClicked() {
    deleteQueryTab();
}

void MainWindow::onTreeSelectionChanged() {
    auto selected = ui->scripttree->currentItem();
    if (selected != nullptr) {
        ui->RemoveToolButton->setEnabled(selected->parent() == nullptr );
        ui->movequerybutton->setEnabled(selected->parent() != nullptr);
    } else {
        ui->RemoveToolButton->setEnabled(false);
        ui->movequerybutton->setEnabled(false);
    }
}

void MainWindow::onTreeDoubleClicked( const QModelIndex &index ) {
    onmovequerybuttonClicked();
}

void MainWindow::onrandomizeredgecountvaluechanged(const int& value) {
    QString s = QString::number((float)value/100);
    ui->querytabs->currentWidget()->findChild<QComboBox*>("randomizeredgecountcombobox")->setCurrentText(s);
}

void MainWindow::onrandomizeredgecountcombochanged(const QString& text) {
    auto i = (int)(text.toFloat()*100);
    ui->querytabs->currentWidget()->findChild<QSlider*>("randomizeredgecountslider")->setValue(i);
    onautocheckrandomizer();
}

void MainWindow::onautocheckrandomizer() {
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("randomizercheckbox")->setChecked(true);
}
void MainWindow::onautocheckgraphfiles() {
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("graphfilescheckbox")->setChecked(true);
}
void MainWindow::onautocheckinlinegraphs() {
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("inlinegraphscheckbox")->setChecked(true);
}

void MainWindow::onAddToolButtonClicked() {
    openQueryFileDialog();
}

void MainWindow::onlogfileopenbuttonClicked() {
    openexternaleditor(this, ui->logfilecombo->currentText());
}

int populatequerysubitems( QTreeWidgetItem * tree, const std::string& fileName ) {


    // Declare an input file stream object
    std::ifstream inputFile(fileName); // Opens the file upon creation
    std::string line;

    // Check if the file was successfully opened
    if (inputFile.is_open()) { // The 'is_open()' function or '!' operator can be used
        // Read the file line by line using std::getline()
        while (std::getline(inputFile, line)) {
            // Print each line to the console
            // std::cout << line << std::endl;
            QTreeWidgetItem *childItem = nullptr;
            if (line.rfind("#", 0) == 0) {
                std::string accumulatestring {};
                accumulatestring += line;
                bool part = true;
                while (part) {
                    part = false;
                    if (std::getline(inputFile, line))
                        part = (line.rfind("#", 0) == 0);
                    if (part)
                        accumulatestring += "; " + line;
                }
                childItem = new QTreeWidgetItem();
                childItem->setText(0,accumulatestring.c_str());
            }
            if (line.size() > 0) {
                if (!childItem)
                    childItem = new QTreeWidgetItem();
                childItem->setText(1,line.c_str());
                tree->addChild(childItem);
            }
        }

        // Close the file stream (good practice, though destructors handle this)
        inputFile.close();
    } else {
        // Output an error message if the file failed to open
        std::cerr << "Unable to open file: " << fileName << std::endl;
        return 1; // Indicate an error
    }

    return 0; // Indicate success
}

int MainWindow::openQueryFileDialog() {

    // The static function returns a QString with the selected file path
    QString fileName = QFileDialog::getOpenFileName(
        this,                                        // Parent widget (e.g., your QMainWindow or QWidget)
        tr("Open Shell Script File"),                             // Dialog title
        DEFAULTDIRECTORY,                      // Default directory (e.g., a specific folder)
        tr("Query shell scripts (*.sh);;All Files (*.*)")    // File filters
    );

    // Check if a file was selected (fileName will be empty if the user cancelled)
    if (!fileName.isEmpty()) {
        // qDebug() << "Selected file path:" << fileName;
        // You can now use this 'fileName' to open and read the file using QFile
        // QTreeWidget *treeWidget = new QTreeWidget();
        // treeWidget->setColumnCount(2);
        ui->scripttree->setHeaderLabel("Comment");
        ui->scripttree->setHeaderLabels({"Comment","Query"});
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem();
        topLevelItem->setText(0,fileName);
        populatequerysubitems( topLevelItem, fileName.toStdString() );
        ui->scripttree->addTopLevelItem(topLevelItem);
        ui->scripttree->expandItem(ui->scripttree->topLevelItem(ui->scripttree->topLevelItemCount()-1));
        return 0;
    }
    return 1;
}

void MainWindow::onRemoveToolButtonClicked() {
    auto todelete = ui->scripttree->currentItem();
    if (todelete != nullptr) {
        if (todelete->parent() == nullptr) {
            ui->scripttree->takeTopLevelItem(ui->scripttree->indexOfTopLevelItem(todelete));
        }
    }
}

void MainWindow::onstoredproceduresbuttonClicked() {
    auto fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Open stored procedure(s)"), // Dialog title
                                                    DEFAULTDIRECTORY,     // Default directory
                                                    tr("Stored procedure files (*.dat);;All files (*.*)"));
    if (!fileNames.isEmpty()) {
        auto c = ui->querytabs->currentWidget()->findChild<QComboBox*>("storedprocedurescombo");
        if (c->currentText() != "")
            c->addItem(c->currentText());
        std::vector<std::string> out =
            parsesemicolondelimeted(c->currentText().toStdString(), STRINGDELIMITER);
        for (auto f : fileNames)
            addstring(out,f.toStdString());
        c->setCurrentText(tosemicolondelimeted(out).c_str());
    }
}

void MainWindow::onpythonbuttonClicked() {
    auto fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Open Python script(s)"), // Dialog title
                                                    DEFAULTDIRECTORY,     // Default directory
                                                    tr("Python files (*.py);;All files (*.*)"));
    if (!fileNames.isEmpty()) {
        auto c = ui->querytabs->currentWidget()->findChild<QComboBox*>("pythoncombo");
        if (c->currentText() != "")
            c->addItem(c->currentText());
        std::vector<std::string> out =
            parsesemicolondelimeted(c->currentText().toStdString(), STRINGDELIMITER);
        for (auto f : fileNames)
            addstring(out,f.toStdString());
        c->setCurrentText(tosemicolondelimeted(out).c_str());
    }
};
void MainWindow::onlogfilebuttonClicked() {


    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Choose a file to append or create a new script file"),
        DEFAULTDIRECTORY,
#ifdef Q_OS_LINUX
        tr("Shell script files (*.sh);;All Files (*.*)"),
#else
        tr("Batch files (*.bat);;All files (*.*)"),
#endif
        nullptr,
        QFileDialog::DontConfirmOverwrite
        );

    if (!filepath.isEmpty()) {
        logfilename = filepath.toStdString();
        ui->logfilecombo->setCurrentText(logfilename.c_str());
    }

}
void MainWindow::onoutfilebuttonClicked() {
    bool overwrite = ui->querytabs->currentWidget()->findChild<QCheckBox*>("outoverwritecheckbox")->checkState();

    QString filepath;
    if (overwrite)
        filepath = QFileDialog::getSaveFileName(
            this,
            tr("Choose a file to overwrite, append or create a new graph output file"),
            DEFAULTDIRECTORY,
            tr("Graph data files (*.dat);;All Files (*.*)"),
            nullptr
            );
    else
        filepath = QFileDialog::getSaveFileName(
            this,
            tr("Choose a file to overwrite, append or create a new graph output file"),
            DEFAULTDIRECTORY,
            tr("Graph data files (*.dat);;All Files (*.*)"),
            nullptr,
            QFileDialog::DontConfirmOverwrite
            );
    if (!filepath.isEmpty()) {
        fcbridges[currentfcindex]->setoutfile(filepath.toStdString());
    }
}

void MainWindow::handleverbosityDialogClose(int result) {
    auto qt = ui->querytabs->currentWidget()->findChild<QComboBox*>("verbositycombo");
    std::vector<std::string> ss {};
    auto vW = verbosityDialog->findChild<QListWidget*>("verbosityListWidget");

    if (result == QDialog::Accepted) {
        for (int i = 0; i < vW->count(); i++) {
            bool found = false;
            if (vW->item(i)->checkState() == Qt::Checked) {
                for (auto j : ss)
                    if (j == vW->item(i)->text().toStdString()) {
                        found = true;
                    }
                if (!found)
                    ss.push_back(vW->item(i)->text().toStdString());
            }
        }
        auto t = tosemicolondelimeted(ss);
        qt->addItem(t.c_str());
        qt->setCurrentText(t.c_str());
    }
}
void MainWindow::onrunandlogbuttonClicked() {runandlogQuery();}
void MainWindow::onlogfileclearbuttonClicked() {}
void MainWindow::onverbositybuttonClicked() {
    if (!verbosityDialog)
        verbosityDialog = new VerbosityDialog(this);

    auto qt = ui->querytabs->currentWidget()->findChild<QComboBox*>("verbositycombo");
    auto ss = parsesemicolondelimeted(qt->currentText().toStdString(), STRINGDELIMITER);
    auto vW = verbosityDialog->findChild<QListWidget*>("verbosityListWidget");

    for (int j = 0; j < vW->count();++j) {
        vW->item(j)->setFlags( vW->item(j)->flags() | Qt::ItemIsUserCheckable);
        vW->item(j)->setCheckState( Qt::Checked );
    }
    for (int j = 0; j < vW->count();++j) {
        vW->item(j)->setCheckState( Qt::Unchecked );
    }
    for (int i = 0; i < ss.size(); i++) {
        for (int j = 0; j < vW->count(); j++) {
            if (vW->item(j)->text() == ss[i]) {
                vW->item(j)->setCheckState(Qt::Checked);
            }
        }
    }
    verbosityDialog->show();
    connect(verbosityDialog, &QDialog::finished, this, &MainWindow::handleverbosityDialogClose);

}
void MainWindow::onoutputfilebuttonClicked() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                            tr("Choose output file"), // Dialog title
                                            DEFAULTDIRECTORY,     // Default directory
                                            tr("Output files (*.dat);;All files (*.*)"));
    if (!fileName.isEmpty()) {
        outputfilename = fileName.toStdString();
    } else
        outputfilename = "";
    ui->outputfilecombobox->setCurrentText(outputfilename.c_str());
    ui->outputfilecombobox->addItem(outputfilename.c_str());
    std::ofstream outfile(outputfilename.c_str(), std::ofstream::out);
    outfile.close(); // has the effect of erasing it
}

void MainWindow::onclearoutputfilebuttonClicked() {
    const std::string filename = outputfilename.c_str();

    std::ofstream ofs(filename, std::ios::out | std::ios::trunc);

    if (!ofs.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
    ofs.close();
}

void MainWindow::onclearoutputbuttonClicked() {
    ui->outputtextedit->clear();
};
void MainWindow::onoutputopenbuttonClicked() {
    openexternaleditor(this, outputfilename.c_str());
};

void MainWindow::onoutfileopenbuttonClicked() {openexternaleditor(this, ui->querytabs->currentWidget()->findChild<QComboBox*>("outfilecombo")->currentText());}
void MainWindow::onrunbuttonClicked() {}

void MainWindow::addNewQueryTab()
{
    // 1. Create an instance of your custom widget/form
    QueryTabWidget *newPage = new QueryTabWidget(this); // Pass 'this' as the parent

    newPage->setTabText(0,"");
    newPage->setTabBarAutoHide(true);

    // 2. Add the widget to the QTabWidget (named 'tabWidget' in this example)
    int max = 0;
    int newTabIndex = ui->querytabs->count();
    for (auto m : fcbridges)
        max = m->visibleindex > max ? m->visibleindex : max;
    ui->querytabs->addTab(newPage, tr("Query %1").arg(max + 1));
    auto fcinstance = new fcinstanceQtbridge(ui,newPage);
    fcinstance->visibleindex = max + 1;
    fcbridges.push_back(fcinstance);

    ui->querytabs->setCurrentIndex(newTabIndex);

    currentqueryedit = ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("queryedit");
    currentcr1edit = ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("cr1edit");
    currentcr2edit = ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("cr2edit");
    currentcr3edit = ui->querytabs->currentWidget()->findChild<QPlainTextEdit*>("cr3edit");

    connect(newPage->findChild<QSlider*>("randomizeredgecountslider"), &QSlider::valueChanged, this, &MainWindow::onrandomizeredgecountvaluechanged);
    connect(newPage->findChild<QComboBox*>("randomizeredgecountcombobox"), &QComboBox::currentTextChanged, this, &MainWindow::onrandomizeredgecountcombochanged);
    connect(newPage->findChild<QComboBox*>("randomizerdimcombobox"), &QComboBox::currentTextChanged, this, &MainWindow::onautocheckrandomizer);
    connect(newPage->findChild<QComboBox*>("randomizercountcombobox"), &QComboBox::currentTextChanged, this, &MainWindow::onautocheckrandomizer);
    connect(newPage->findChild<QComboBox*>("randomizercombobox"), &QComboBox::currentTextChanged, this, &MainWindow::onautocheckrandomizer);
    newPage->findChild<QSlider*>("randomizeredgecountslider")->setValue(50);
    connect(newPage->findChild<QComboBox*>("graphfilescombo"), &QComboBox::currentTextChanged, this, &MainWindow::onautocheckgraphfiles);
    connect(newPage->findChild<QComboBox*>("inlinegraphscombo"), &QComboBox::currentTextChanged, this, &MainWindow::onautocheckinlinegraphs);
    connect(newPage->findChild<QComboBox*>("outfilecombo"), &QComboBox::currentTextChanged, this, &MainWindow::onoutchanged);
    connect(newPage->findChild<QCheckBox*>("outfilecheckbox"), &QCheckBox::checkStateChanged, this, &MainWindow::onoutfilecheckboxchanged);

    onoutfilecheckboxchanged(false);
    auto q = newPage->findChild<QPlainTextEdit*>("queryedit");
    auto cr1 = newPage->findChild<QPlainTextEdit*>("cr1edit");
    auto cr2 = newPage->findChild<QPlainTextEdit*>("cr2edit");
    auto cr3 = newPage->findChild<QPlainTextEdit*>("cr3edit");

    logsyntaxhighlightchange( q );
    logsyntaxhighlightchange( cr1 );
    logsyntaxhighlightchange( cr2 );
    logsyntaxhighlightchange( cr3 );

    connect(q, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onquerycriteditcursorpositionchanged);
    connect(cr1, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onquerycriteditcursorpositionchanged);
    connect(cr2, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onquerycriteditcursorpositionchanged);
    connect(cr3, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onquerycriteditcursorpositionchanged);

    connect(q, &QPlainTextEdit::textChanged, this, &MainWindow::onquerycritedittextchanged);
    connect(cr1, &QPlainTextEdit::textChanged, this, &MainWindow::onquerycritedittextchanged);
    connect(cr2, &QPlainTextEdit::textChanged, this, &MainWindow::onquerycritedittextchanged);
    connect(cr3, &QPlainTextEdit::textChanged, this, &MainWindow::onquerycritedittextchanged);

    connect(newPage->findChild<QToolButton*>("storedprocedurefilesbutton"), &QToolButton::clicked, this, &MainWindow::onstoredproceduresbuttonClicked);
    connect(newPage->findChild<QToolButton*>("pythonfilesbutton"), &QToolButton::clicked, this, &MainWindow::onpythonbuttonClicked);
    connect(newPage->findChild<QToolButton*>("outfilebutton"), &QToolButton::clicked, this, &MainWindow::onoutfilebuttonClicked);
    connect(newPage->findChild<QToolButton*>("verbositybutton"), &QToolButton::clicked, this, &MainWindow::onverbositybuttonClicked);

    connect(newPage->findChild<QToolButton*>("outfileopenbutton"), &QToolButton::clicked, this, &MainWindow::onoutfileopenbuttonClicked);
    connect( newPage->findChild<QToolButton*>("graphfilesbutton"), &QToolButton::clicked, this, &MainWindow::ongraphfilesbuttonclicked);

}


void MainWindow::deleteQueryTab()
{
    int index = ui->querytabs->currentIndex();
    if (ui->querytabs->count() > 1) {
        QWidget* tabWidgetToRemove = ui->querytabs->widget(index);
        ui->querytabs->removeTab(index);
        delete tabWidgetToRemove; // Manually delete the widget
        delete fcbridges[index];
        fcbridges.erase(fcbridges.begin() + index);
    }
}

void MainWindow::runQuery() {int index = ui->querytabs->currentIndex();fcbridges[index]->runQuery();}
void MainWindow::logQuery() {int index = ui->querytabs->currentIndex();fcbridges[index]->logQuery();}
void MainWindow::runandlogQuery() {int index = ui->querytabs->currentIndex(); fcbridges[index]->runandlogQuery();}

void MainWindow::onoutchanged() {
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("outfilecheckbox")->setChecked(true);
}
void MainWindow::onoutfilecheckboxchanged(bool clicked) {
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("outoverwritecheckbox")->setDisabled(!clicked);
    ui->querytabs->currentWidget()->findChild<QCheckBox*>("outpassedcheckbox")->setDisabled(!clicked);
}

void logsyntaxhighlightchange( QPlainTextEdit* textedit ) {
    QTextCursor cursor = textedit->textCursor();
    QTextBlock block = cursor.block();


    BlockData* pi;
    if (block.userData()) {
        pi = static_cast<BlockData*>(block.userData());
    } else
        pi = new BlockData();
    auto t = textedit->toPlainText();
    std::vector<int> nestcurl, nestsquare, nestcurly {};
    pi->parentheses.clear();
    for (int i = 0; i < t.size(); ++i) {
        ParenthesisInfo p;

        p.position = -1;
        if (t.at(i) == '(') {
            p.character = '(';
            nestcurl.push_back(i);
        } else if (t.at(i) == ')') {
            p.character = ')';
            if (!nestcurl.empty()) {
                p.position = nestcurl[nestcurl.size() - 1];
                pi->parentheses[p.position].position = i;
                nestcurl.pop_back();
            }
        } else if (t.at(i) == ']') {
            p.character = ']';
            if (!nestsquare.empty()) {
                p.position = nestsquare[nestsquare.size() - 1];
                pi->parentheses[p.position].position = i;
                nestsquare.pop_back();
            }
        } else if (t.at(i) == '[') {
            p.character = '[';
            nestsquare.push_back(i);
        } else if (t.at(i) == '}') {
            p.character = '}';
            if (!nestcurl.empty()) {
                p.position = nestcurly[nestcurly.size() - 1];
                pi->parentheses[p.position].position = i;
                nestcurly.pop_back();
            }
        } else if (t.at(i) == '{') {
            p.character = '{';
            nestcurly.push_back(i);
        }
        pi->parentheses.push_back(p);
    }
    block.setUserData(static_cast<QTextBlockUserData*>(pi));
}

void doquerytextpositionchanged( const MainWindow* m, QObject* sender ) {
    // auto querycritedit = qobject_cast<QPlainTextEdit*>(sender);
    // auto cursor = querycritedit->textCursor();
    // QTextBlock block = cursor.block();
    // int pos = cursor.position();
    // QString text = m->currentqueryedit->toPlainText();
    for (auto h : m->fcbridges[m->currentfcindex]->highlighters)
        h->rehighlight();
}

void MainWindow::onquerycritedittextchanged() {
    logsyntaxhighlightchange( qobject_cast<QPlainTextEdit*>(sender()) );
    // doquerytextpositionchanged(this,sender());
}

void MainWindow::onquerycriteditcursorpositionchanged() {
    doquerytextpositionchanged(this,sender());
}

void MainWindow::onQueryTabCurrentChanged(int pos ) {
    currentfcindex = pos;
}

void MainWindow::onlogfilecombocurrenttextchanged() {
    ui->logfileopenbutton->setEnabled(!ui->logfilecombo->currentText().isEmpty());
}
void MainWindow::ongraphfilesbuttonclicked() {
    auto fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Open graph data file(s)"), // Dialog title
                                                    DEFAULTDIRECTORY,     // Default directory
                                                    tr("Graph data files (*.dat);;All files (*.*)"));
    if (!fileNames.isEmpty()) {
        auto c = ui->querytabs->currentWidget()->findChild<QComboBox*>("graphfilescombo");
        if (c->currentText() != "")
            c->addItem(c->currentText());
        std::vector<std::string> out =
            parsesemicolondelimeted(c->currentText().toStdString(), STRINGDELIMITER);
        for (auto f : fileNames)
            addstring(out,f.toStdString());
        c->setCurrentText(tosemicolondelimeted(out).c_str());
    }

}

void MainWindow::onmovequerybuttonClicked() {
    auto selected = ui->scripttree->currentItem();
    if (selected != nullptr) {
        if (selected->parent() != nullptr) {
            addNewQueryTab();
            auto s = selected->data(1,Qt::DisplayRole).toString().toStdString();
            // std::cout << s << std::endl;
            fcbridges[fcbridges.size()-1]->fc.reverseparse(s);
            fcbridges[fcbridges.size()-1]->passparameterstowidgets();

        }
    }

}
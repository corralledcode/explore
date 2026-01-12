#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "fcinstance.h"
#include <iostream>
#include <fstream>


void trim(std::string& str) {
    // Find the first non-whitespace character from the beginning
    auto first = std::find_if_not(str.begin(), str.end(), ::isspace);
    str.erase(str.begin(), first); // Erase leading whitespace

    // Find the first non-whitespace character from the end (use rbegin/rend)
    auto last = std::find_if_not(str.rbegin(), str.rend(), ::isspace);
    str.erase(last.base(), str.end()); // Erase trailing whitespace
}

void loadConfig() {
    std::ifstream configFile(EXPLORECONFIGFILENAME);
    if (configFile.is_open()) {
        std::string line {};
        while (std::getline(configFile, line)) {
            size_t pos = line.find("=");
            if (pos != std::string::npos) {
                auto left = line.substr(0,pos);
                trim(left);
                auto right = line.substr(pos+1);
                trim(right);
                if (left == "FLAGCALCEXECUTABLEPATH") {
                    FLAGCALCEXECUTABLEPATH = right;
                }
                if (left == "FLAGCALCEXECUTABLENAME") {
                    FLAGCALCEXECUTABLENAME = right;
                }
                if (left == "FLAGCALCHEADER") {
                    FLAGCALCHEADER = right;
                }
                if (left == "FLAGCALCINVOKE") {
                    FLAGCALCINVOKE = right;
                }
                if (left == "DEFAULTDIRECTORY") {
                    DEFAULTDIRECTORY = right;
                }
            }
        }
        configFile.close();
    }
}

int main(int argc, char *argv[])
{
    fcinstance fc;

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "explore_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    loadConfig();
    MainWindow w(fc);
    w.show();
    return a.exec();
}

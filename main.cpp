#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "fcinstance.h"


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
    MainWindow w(fc);
    w.show();
    return a.exec();
}

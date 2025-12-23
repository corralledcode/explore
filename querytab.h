//
// Created by peterglenn on 12/21/25.
//

#ifndef EXPLORE_QUERYTAB_H
#define EXPLORE_QUERYTAB_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class QueryTabWidget;
}
QT_END_NAMESPACE


class QueryTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit QueryTabWidget(QWidget *parent = nullptr);
    ~QueryTabWidget();
private:
    Ui::QueryTabWidget *ui;
};


#endif //EXPLORE_QUERYTAB_H
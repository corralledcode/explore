//
// Created by peterglenn on 12/21/25.
//

#include "querytab.h"

#include "./ui_querytab.h"

#include "querytab.h"





QueryTabWidget::QueryTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::QueryTabWidget) {
    ui->setupUi(this);
    // Set other widget properties, signals, and slots here
    // For example: connect(button, &QPushButton::clicked, this, &MyCustomTabWidget::onButtonClicked);
}

QueryTabWidget::~QueryTabWidget()
{
    delete ui;
}
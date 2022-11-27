#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FortTest.h"

class FortTest : public QMainWindow
{
    Q_OBJECT

public:
    FortTest(QWidget *parent = nullptr);
    ~FortTest();

public:


private:
    Ui::FortTestClass ui;
};

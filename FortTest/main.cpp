#include "FortTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FortTest w;
    w.show();
    return a.exec();
}

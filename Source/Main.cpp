#include "printmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PrintManager w;
    w.show();

    return a.exec();
}

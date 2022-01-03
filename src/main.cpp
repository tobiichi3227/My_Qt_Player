#include "My_Qt_Player.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    My_Qt_Player w;
    w.show();
    return a.exec();
}

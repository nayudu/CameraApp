#include "AppUI.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppUI w;
    w.show();

    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    a.setStyleSheet("QSplitter::handle { background-color: gray }");

    //w.MainLayout();

    return a.exec();
}



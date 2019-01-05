#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qApp->addLibraryPath(qApp->applicationDirPath() + "./plugins");
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}

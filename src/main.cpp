/**************************
 * Sample project for show clock and timer always on top of screen
 *************************/

#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.show();
    return a.exec();
}

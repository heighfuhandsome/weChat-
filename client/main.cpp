#include "widget.h"
#include "searchwid.h"
#include <QFile>
#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   Widget w;
   w.setWindowFlags(w.windowFlags() & ~Qt::WindowContextHelpButtonHint);
   w.show();
    return a.exec();
}

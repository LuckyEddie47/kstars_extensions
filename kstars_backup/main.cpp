#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Backup");
    MainWindow w(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()));
    w.show();
    return app.exec();
}

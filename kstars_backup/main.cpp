#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Backup");
    QString ks_version = "";
    if (app.arguments().count() == 2) {
        ks_version = app.arguments().at(1);
    }
    MainWindow w(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()), ks_version);

    QObject::connect(&w, &MainWindow::quit, &app, &QApplication::quit);

    w.show();
    return app.exec();
}

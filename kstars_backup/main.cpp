#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("KS_backup");
    QString ks_version = "";
    if (app.arguments().count() == 2) {
        ks_version = app.arguments().at(1);
    }
    MainWindow w(QString("%1%2%3").arg(app.applicationDirPath(), "/", app.applicationName()), ks_version);
    w.show();

    QMessageBox m_msgbox(QMessageBox::Question,
                         QObject::tr("Close KStars?"),
                         QObject::tr("To prevent files being used during the backup/restore "
                                     "process it is necessary to close KStars.\n"
                                     "Okay to proceed?"),
                         QMessageBox::Yes | QMessageBox::No);
    m_msgbox.setDefaultButton(QMessageBox::No);
    if (m_msgbox.exec() == QMessageBox::Yes) {
        w.begin();
    } else {
        QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
    }

    QObject::connect(&app, &QApplication::aboutToQuit, &w, &MainWindow::halt);

    return app.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator m_translator;
    if (m_translator.load(QLocale(), ":/i18n/KS_backup")) {
        app.installTranslator(&m_translator);
    }
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
                                     "Okay to proceed?"));
    QPushButton *yesButton = m_msgbox.addButton(QObject::tr("Yes"), QMessageBox::YesRole);
    QPushButton *noButton = m_msgbox.addButton(QObject::tr("No"), QMessageBox::NoRole);
    Q_UNUSED(*noButton);
    m_msgbox.exec();

    if (m_msgbox.clickedButton() == yesButton) {
        w.begin();
    } else {
        QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
    }

    QObject::connect(&app, &QApplication::aboutToQuit, &w, &MainWindow::halt);

    return app.exec();
}

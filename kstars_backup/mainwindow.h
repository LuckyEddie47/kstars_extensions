#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

#include "archiver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    archiver* m_archiver;

    typedef enum modeType {
        MODE_BACKUP,
        MODE_RESTORE
    } modeType;

    modeType mode = MODE_BACKUP;

    QStringListModel* m_model;
};
#endif // MAINWINDOW_H

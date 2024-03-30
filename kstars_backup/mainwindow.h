#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

#include "archiver.h"
#include "confchecker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &appFilePath, const QString &ks_version = "", QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    confChecker* m_confChecker;
    archiver* m_archiver;
    QStringListModel* m_model;
    QStringList* chosenPaths;

    void setNewPath(const QString &path);

    typedef enum modeType {
        MODE_BACKUP,
        MODE_RESTORE
    } modeType;
    modeType mode = MODE_BACKUP;

    QString lastDir = "/tmp";
    ulong archiveSize, spaceNeeded, spaceAvailable;

};
#endif // MAINWINDOW_H

/* This is an example process class for KStar_extensions
 * It does nothing except emit messages which can be
 * passed back to KStars
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QTimer>

class process : public QObject
{
    Q_OBJECT
public:
    explicit process(QObject *parent = nullptr);
    void startProcess();
    void stopProcess();

signals:
    void message(const QString output);

private:
    QTimer* m_timer;
};

#endif // PROCESS_H

/* This is an exmaple logger class that just outputs the passed
 * messages to qDebug()
 */

#ifndef LOG_H
#define LOG_H

#include <QObject>

class logger : public QObject
{
    Q_OBJECT
public:
    explicit logger(QObject *parent = nullptr);
    void out(const QString &output);
};

#endif // LOG_H

#ifndef LOG_H
#define LOG_H

#include <QObject>

class logger : public QObject
{
    Q_OBJECT
public:
    explicit logger(QObject *parent = nullptr);
    void out(QString);

private:

signals:

};

#endif // LOG_H

#ifndef TESTER_H
#define TESTER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QHostAddress>
#include <QDebug>

#define FREQ 0.5
#define TS   10
#define PI2  (2.0 * 3.141592654)

class Tester : public QObject
{
    Q_OBJECT
public:
    explicit Tester(QObject *parent = 0);

signals:
    void dataReceived(QByteArray, int);

public slots:
    void start()
    {
        sendOne();
    }

private slots:
    void sendOne()
    {
        static double n = 0;
        QDateTime dt = QDateTime::currentDateTime();
        double t = dt.toMSecsSinceEpoch() / 1000.0;
        {
            double d = sin(t * FREQ * 2.0 * 3.141592654);
            QString data = QString("n-:Test.sin v-:%1 t-:%2\r\n").arg(d).arg(t, 0, 'f', 16);
            emit dataReceived(data.toLocal8Bit(), QHostAddress(QHostAddress::LocalHost).toIPv4Address());
        }
        {
            double d = cos(t * FREQ * 2.0 * 3.141592654);
            QString data = QString("n-:Test.cos v-:%1 t-:%2\r\n").arg(d).arg(t, 0, 'f', 16);
            emit dataReceived(data.toLocal8Bit(), QHostAddress(QHostAddress::LocalHost).toIPv4Address());
        }

        QTimer::singleShot(TS, this, SLOT(sendOne()));
    }
};

#endif // TESTER_H

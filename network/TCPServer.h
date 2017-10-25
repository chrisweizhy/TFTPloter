#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <qglobal.h>

#include <QTcpServer>

#include <loki/Singleton.h>

class _TCPServer : public QObject
{
    Q_OBJECT
public:
    explicit _TCPServer(QObject *parent = 0);

    void start();

signals:
    void dataReceived(QByteArray, int);
    void socketDisconnected(int);

public slots:

private slots:
    void on_loaded();
    void on_newConnection();
    void on_socketClosed();
    void on_socketReadyRead();

private:
    QTcpServer* mServer;
};

typedef ::Loki::SingletonHolder<_TCPServer> TCPServer;

#endif // TCPSERVER_H

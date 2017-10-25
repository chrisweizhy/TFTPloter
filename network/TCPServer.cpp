#include "TCPServer.h"

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QHostAddress>

extern quint16 gListenPort;

_TCPServer::_TCPServer(QObject *parent) : QObject(parent)
{
}

void _TCPServer::start()
{
    QThread* thread = new QThread;
    this->moveToThread(thread);
    thread->start();

    QTimer* timer = new QTimer;
    timer->setSingleShot(true);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_loaded()), Qt::QueuedConnection);
    timer->start();
}

void _TCPServer::on_loaded()
{
    mServer = new QTcpServer;
    connect(mServer, SIGNAL(newConnection()), this, SLOT(on_newConnection()));
    mServer->listen(QHostAddress::Any, gListenPort);
}

void _TCPServer::on_newConnection()
{
    QTcpSocket* pSocket = mServer->nextPendingConnection();
    connect(pSocket, SIGNAL(readyRead()), this, SLOT(on_socketReadyRead()));
    connect(pSocket, SIGNAL(disconnected()), this, SLOT(on_socketClosed()));
}

void _TCPServer::on_socketClosed()
{
    QTcpSocket* pSocket = qobject_cast<QTcpSocket*>(sender());
    disconnect(pSocket, SIGNAL(readyRead()), this, SLOT(on_socketReadyRead()));
    disconnect(pSocket, SIGNAL(disconnected()), this, SLOT(on_socketClosed()));
    emit socketDisconnected(pSocket->peerAddress().toIPv4Address());
}

void _TCPServer::on_socketReadyRead()
{
    QTcpSocket* pSocket = qobject_cast<QTcpSocket*>(sender());
    emit dataReceived(pSocket->readAll(), pSocket->peerAddress().toIPv4Address());
}


#include "MainWindow.h"
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QString>

#include "TCPServer.h"
#include "DataParser.h"
#include "tester.h"

quint16 gListenPort = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef Q_OS_WIN
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
#endif
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    //check and get port
    if (argc < 2){
        int port = QInputDialog::getInt(0, "Input", "Please input the TCP port to listen:", 6666, 1024, 65535);
        if (port > 1024 && port < 65535)
            gListenPort = port;
        else
            return 0;
    }

    //start tcp server
    TCPServer::Instance().start();
    QObject::connect(&TCPServer::Instance(), SIGNAL(dataReceived(QByteArray,int)), &DataParser::Instance(), SLOT(on_dataReceived(QByteArray,int)), Qt::QueuedConnection);

    //main window
    MainWindow w;
    w.showMaximized();

    //Test
    Tester tester;
    QObject::connect(&tester, SIGNAL(dataReceived(QByteArray,int)), &DataParser::Instance(), SLOT(on_dataReceived(QByteArray,int)), Qt::QueuedConnection);
    foreach(QString arg, QApplication::arguments()){
        if (arg == QString("-t")){
            tester.start();
            break;
        }
    }

    return a.exec();
}

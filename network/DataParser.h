#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QHash>
#include <QMap>
#include <QObject>
#include <qglobal.h>

#include <loki/Singleton.h>

#include "DataProvider.h"
#include <qstringlist.h>

class _DataParser : public QObject
{
    Q_OBJECT
public:
    explicit _DataParser(QObject *parent = 0);

    void registerObserver(QString name, QObject* pObj);

    inline QStringList names()
    {
        return mProviderHash.keys();
    }

signals:
    void newName(QString name);
    void delName(QString name);

public slots:
    void on_dataReceived(QByteArray data, int ip);
    void on_socketDisconnected(int ip);

private:
    QMap<int, QString> mSaveData;
    QHash<QString, DataProvider*> mProviderHash;
    QMap<int, QStringList> mIPMapToNameList;
};

typedef ::Loki::SingletonHolder<_DataParser> DataParser;

#endif // DATAPARSER_H

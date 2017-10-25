#include "DataParser.h"

#include <QDebug>
#include <QVariant>
#include <QDateTime>

_DataParser::_DataParser(QObject *parent) : QObject(parent)
{

}

void _DataParser::registerObserver(QString name, QObject* pObj)
{
    if (mProviderHash.contains(name)){
        if (!connect(mProviderHash[name], SIGNAL(newData(QString,double,double)), pObj, SLOT(on_dataReady(QString,double,double))))
            qWarning() << "name failed.";
    }
}

void _DataParser::on_dataReceived(QByteArray data, int ip)
{
    if (!mSaveData.contains(ip)){
        mSaveData[ip] = "";
    }

    QString& strSaved = mSaveData[ip];
    QString input = strSaved + QString::fromLocal8Bit(data.data());
    input = input.replace("\r", "");

    QStringList sl = input.split("\n", QString::SkipEmptyParts);
    bool hasLeft = !input.endsWith("\n");

    strSaved = hasLeft ? sl.last() : "";

    for (int i=0; i<sl.count()-(hasLeft ? 1 : 0); i++){
        QStringList ss = sl[i].split(" ", QString::SkipEmptyParts);
        QHash<QString, QVariant> vh;
        foreach(QString s, ss){
            {
                if (s.startsWith("n-:")){
                    vh["name"] = QVariant::fromValue(s.mid(3));
                    continue;
                }
            }
            {
                if (s.startsWith("t-:")){
                    vh["time"] = QVariant::fromValue(s.mid(3).toDouble());
                }
                if (s.startsWith("T-:")){
                    vh["time"] = QVariant::fromValue(QDateTime(QDate::currentDate(), QTime::fromString(s.mid(3), "HH:mm:ss.zzz")).toTime_t());
                    continue;
                }

                if (s.startsWith("TT-:")){
                    vh["time"] = QVariant::fromValue(QDateTime::fromString(s.mid(4), "YYYY-MM-ddTHH:mm:ss.zzz").toTime_t());
                    continue;
                }
            }
            {
                if (s.startsWith("v-:")){
                    vh["value"] = QVariant::fromValue(s.mid(3).toDouble());
                    continue;
                }
            }
        }

//        QStringList res;
//        foreach(QString key, vh.keys()){
//            res << QString("%1=%2").arg(key).arg(vh[key].toString());
//        }
//        qDebug() << ">> " << qPrintable(sl[i]);
//        if (!res.isEmpty())
//            qDebug() << qPrintable(QString(res.join(", ")));

        if (vh.contains("name") && vh.contains("value")){
            QString name  = vh["name"].toString();
            double  value = vh["value"].toDouble();


            double     n;
            if (vh.contains("time"))
                n = vh["time"].toDouble();
            else
                n = QDateTime::currentMSecsSinceEpoch() / 1000.0;

            QString tagName = name;
            QString subName = "";
            if (name.contains(".")){
                tagName = name.left(name.indexOf("."));
                subName = name.mid(name.indexOf(".") + 1);
            }

            //valid data
            if (!mProviderHash.contains(tagName)){
                mProviderHash[tagName] = new DataProvider;
                mIPMapToNameList[ip] << tagName;
                emit newName(tagName);
            }
            mProviderHash[tagName]->pushData(subName, n, value);
        }
    }
}

void _DataParser::on_socketDisconnected(int ip)
{
    if (mSaveData.contains(ip)){
        mSaveData.take(ip);
    }

    foreach(QString name, mIPMapToNameList[ip]){
        if (mProviderHash.contains(name)){
            emit delName(name);
            delete mProviderHash.take(name);
        }
    }
    mIPMapToNameList.take(ip);
}


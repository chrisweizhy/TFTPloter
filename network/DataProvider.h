#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <qglobal.h>

class DataProvider : public QObject
{
    Q_OBJECT
public:
    explicit DataProvider(QObject *parent = 0);

    inline void pushData(QString subName, double secsFromUTC, double value)
    {
        emit newData(subName, secsFromUTC, value);
    }

signals:
    void newData(QString subName, double secsFromUTC, double value);

public slots:
};

#endif // DATAPROVIDER_H

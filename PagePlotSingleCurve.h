﻿#ifndef PAGEPLOTSINGLECURVE_H
#define PAGEPLOTSINGLECURVE_H

#include <QObject>
#include <qglobal.h>
#include <QFile>
#include <QDataStream>

#include "FormPlot.h"

class PlotData
{
public:
    void clear()
    {
        //curve removed by FormPlot
        mFile->close();
        delete mStream;
        delete mFile;
    }

public:
    QCPGraph* mCurve;

    //utils
    bool mDirty;

    //save files
    QFile* mFile;
    QDataStream* mStream;

};

class PagePlotSingleCurve : public QWidget
{
    Q_OBJECT
public:
    explicit PagePlotSingleCurve(QWidget *parent = 0);

    inline QWidget* widget()
    {
        return (QWidget*)mPlot;
    }

signals:

public slots:
    void on_dataReady(QString subName, double secsFromUTC, double value);

private slots:
    void on_update();
    void on_curveRemoved(QCPGraph* pCurve)
    {
        QString selectedName = "";
        foreach(QString subName, mCurves.keys()){
            if (mCurves[subName].mCurve == pCurve){
                mCurves[subName].clear();
                selectedName = subName;
            }
        }
        if (!selectedName.isEmpty())
            mCurves.remove(selectedName);
    }

    void on_allCurveRemoved()
    {
        foreach(QString subName, mCurves.keys()){
            mCurves[subName].clear();
        }
        mCurves.clear();
    }

private:
    FormPlot* mPlot;

    QHash<QString, PlotData> mCurves;

    QCPGraph* mCurve;
    //
    QVector<double> keyX;
    QVector<double> valueX;

    //utils
    bool mDirty;
    QTimer   mTimer;

    //save files
    QFile mFile;
    QDataStream mStream;
private:
    static QList<QPen> mGPens;
    int mCurrentIndex;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *ev);
};

#endif // PAGEPLOTSINGLECURVE_H
 

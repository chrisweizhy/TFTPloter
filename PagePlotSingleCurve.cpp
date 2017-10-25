#include "PagePlotSingleCurve.h"

#include <QVBoxLayout>
#include <QMessageBox>

#include "DataParser.h"

QList<QPen> PagePlotSingleCurve::mGPens = QList<QPen>();

PagePlotSingleCurve::PagePlotSingleCurve(QWidget *parent) : QWidget(parent), mCurrentIndex(0)
{
    mPlot = new FormPlot;
    connect(mPlot, SIGNAL(curveRemoved(QCPGraph*)), this, SLOT(on_curveRemoved(QCPGraph*)));
    connect(mPlot, SIGNAL(allCurveRemoved()), this, SLOT(on_allCurveRemoved()));

    //mPlot->customplot->legend->setVisible(false);
    QPen penUWBKF;    penUWBKF.setColor(Qt::black);

    mPlot->customplot->yAxis->setLabel("X (m)");
    mPlot->customplot->xAxis->setLabel("Time(Local)");

//    mPlot->customplot->addGraph();
//    mCurve = mPlot->customplot->graph();
//    mCurve->setPen(penUWBKF);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(mPlot);
    this->setLayout(layout);
    layout->setSpacing(0);
    layout->setMargin(0);

    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("HH:mm:ss\nyy-MM-dd");
    mPlot->customplot->xAxis->setTicker(dateTicker);

    mTimer.setInterval(50);
    mTimer.setSingleShot(false);
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(on_update()));

//    mDirty = false;

    if (mGPens.isEmpty()){
        QPen pen;
        pen.setColor(Qt::blue       );    mGPens << pen;
        pen.setColor(Qt::red        );    mGPens << pen;
        pen.setColor(Qt::black      );    mGPens << pen;
        pen.setColor(Qt::green      );    mGPens << pen;
        pen.setColor(Qt::gray       );    mGPens << pen;
        pen.setColor(Qt::cyan       );    mGPens << pen;
        pen.setColor(Qt::magenta    );    mGPens << pen;
        pen.setColor(Qt::yellow     );    mGPens << pen;
        pen.setColor(Qt::darkRed    );    mGPens << pen;
        pen.setColor(Qt::darkGreen  );    mGPens << pen;
        pen.setColor(Qt::darkBlue   );    mGPens << pen;
        pen.setColor(Qt::darkCyan   );    mGPens << pen;
        pen.setColor(Qt::darkMagenta);    mGPens << pen;
        pen.setColor(Qt::darkYellow );    mGPens << pen;
    }
}

void PagePlotSingleCurve::on_dataReady(QString subName, double secsFromUTC, double value)
{
    //global
    if (!mCurves.contains(subName)){
        PlotData& data = mCurves[subName];

        mPlot->customplot->addGraph();
        data.mCurve = mPlot->customplot->graph();
        data.mCurve->setName(subName);
        data.mDirty = false;
        data.mFile = new QFile;
        data.mStream = new QDataStream;

        data.mCurve->setPen(mGPens[(mCurrentIndex++) % mGPens.count()]);
    }

    //per subName
    PlotData& data = mCurves[subName];
    data.mCurve->addData(secsFromUTC, value);

    if (!data.mFile->isOpen()){
        data.mFile->setFileName(QString("%1_%2_%3.dat").arg(this->objectName()).arg(subName).arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")));
        if (data.mFile->open(QIODevice::WriteOnly))
            data.mStream->setDevice(data.mFile);
    }

    if (data.mFile->isOpen()){
        (*data.mStream) << secsFromUTC << value;
        data.mFile->flush();
    }

    data.mDirty = true;

    //global
    if (!mTimer.isActive())
        mTimer.start();
}

void PagePlotSingleCurve::on_update()
{
    bool dirty = false;
    foreach(QString subName, mCurves.keys()){
        dirty |= mCurves[subName].mDirty;
        mCurves[subName].mDirty = false;

        if (dirty)
            break;
    }

    if (dirty){
        mPlot->customplot->rescaleAxes(true);
        mPlot->customplot->replot();
    }
    else
        mTimer.stop();
}

void PagePlotSingleCurve::contextMenuEvent(QContextMenuEvent *ev)
{
    QMessageBox::information(this, "ahah", "HAHAH");
}

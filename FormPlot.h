#ifndef FORMPLOT_H
#define FORMPLOT_H

#include <QWidget>

#include "qcustomplot.h"

class FormPlot : public QWidget
{
    Q_OBJECT

public:
    explicit FormPlot(QWidget *parent = 0);
    ~FormPlot();

    void setPlotTitle(QString plotTitle);
    QString plotTitle() const;

signals:
    void curveRemoved(QCPGraph* pCurve);
    void allCurveRemoved();

private slots:
  void titleDoubleClick(QMouseEvent *event);
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void addRandomGraph();
  void removeSelectedGraph();
  void removeAllGraphs();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

public:
  QCustomPlot* customplot;

private:
  QString mPlotTitle;
};

#endif // FORMPLOT_H

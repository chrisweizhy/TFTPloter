#include "FormPlot.h"

#include <QVBoxLayout>
#include <QInputDialog>
#include <QMenu>

#include "qcustomplot.h"

FormPlot::FormPlot(QWidget *parent) :
    QWidget(parent)
{
    //
    customplot = new QCustomPlot(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(customplot);
    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
    //customplot->removeItem()

    //
    srand(QDateTime::currentDateTime().toTime_t());

    customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    customplot->xAxis->setRange(-8, 8);
    customplot->yAxis->setRange(-5, 5);
    customplot->axisRect()->setupFullAxesBox();

//    customplot->plotLayout()->insertRow(0);
//    QCPTextElement *title = new QCPTextElement(customplot, mPlotTitle, QFont("sans", 17, QFont::Bold));
//    customplot->plotLayout()->addElement(0, 0, title);

    customplot->xAxis->setLabel("x Axis");
    customplot->yAxis->setLabel("y Axis");
    customplot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    customplot->legend->setFont(legendFont);
    customplot->legend->setSelectedFont(legendFont);
    customplot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    customplot->legend->setVisible(true);

    addRandomGraph();
    addRandomGraph();
    addRandomGraph();
    addRandomGraph();
    customplot->rescaleAxes();

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(customplot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(customplot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(customplot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(customplot->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customplot->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(customplot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(customplot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
//    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(customplot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    // setup policy and connect slot for context menu popup:
    customplot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(customplot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

FormPlot::~FormPlot()
{
}

void FormPlot::titleDoubleClick(QMouseEvent* event)
{
    Q_UNUSED(event)
    if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
  {
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "TFT远程数据接收-曲线绘制工具", "新绘图名称:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      customplot->replot();
    }
  }
}

void FormPlot::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "TFT远程数据接收-曲线绘制工具", "新坐标名称:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      customplot->replot();
    }
  }
}

void FormPlot::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "TFT远程数据接收-曲线绘制工具", "新曲线名称:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      customplot->replot();
    }
  }
}

void FormPlot::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customplot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customplot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customplot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customplot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customplot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customplot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customplot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customplot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customplot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customplot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customplot->graphCount(); ++i)
  {
    QCPGraph *graph = customplot->graph(i);
    QCPPlottableLegendItem *item = customplot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void FormPlot::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (customplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customplot->axisRect()->setRangeDrag(customplot->xAxis->orientation());
  else if (customplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customplot->axisRect()->setRangeDrag(customplot->yAxis->orientation());
  else
    customplot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void FormPlot::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customplot->axisRect()->setRangeZoom(customplot->xAxis->orientation());
  else if (customplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customplot->axisRect()->setRangeZoom(customplot->yAxis->orientation());
  else
    customplot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void FormPlot::addRandomGraph()
{
//  int n = 50; // number of points in graph
//  double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
//  double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
//  double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
//  double yOffset = (rand()/(double)RAND_MAX - 0.5)*10;
//  double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
//  double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
//  double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
//  double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
//  QVector<double> x(n), y(n);
//  for (int i=0; i<n; i++)
//  {
//    x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
//    y[i] = (qSin(x[i]*r1*5)*qSin(qCos(x[i]*r2)*r4*3)+r3*qCos(qSin(x[i])*r4*2))*yScale + yOffset;
//  }

//  customplot->addGraph();
//  customplot->graph()->setName(QString("New graph %1").arg(customplot->graphCount()-1));
//  customplot->graph()->setData(x, y);
//  customplot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
//  if (rand()%100 > 50)
//    customplot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
//  QPen graphPen;
//  graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
//  graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
//  customplot->graph()->setPen(graphPen);
//  customplot->replot();
}

void FormPlot::removeSelectedGraph()
{
  if (customplot->selectedGraphs().size() > 0)
  {
      QCPGraph* pCurve = customplot->selectedGraphs().first();
    customplot->removeGraph(pCurve);
    emit curveRemoved(pCurve);
    customplot->replot();
  }
}

void FormPlot::removeAllGraphs()
{
  customplot->clearGraphs();
  emit allCurveRemoved();
  customplot->replot();
}

void FormPlot::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (customplot->legend->selectTest(pos, false) >= 0){ // context menu on legend requested
    menu->addAction(QString::fromLocal8Bit("移动到左上角"  ), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction(QString::fromLocal8Bit("移动到中间靠上"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction(QString::fromLocal8Bit("移动到右上角"  ), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    menu->addAction(QString::fromLocal8Bit("移动到右下角"  ), this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction(QString::fromLocal8Bit("移动到左下角"  ), this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
  }
  else{  // general context menu on graphs requested
//    menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
    if (customplot->selectedGraphs().size() > 0)
      menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    if (customplot->graphCount() > 0)
      menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
  }

  menu->popup(customplot->mapToGlobal(pos));
}

void FormPlot::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      customplot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      customplot->replot();
    }
  }
}

void FormPlot::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  //ui->statusBar->showMessage(message, 2500);
}

void FormPlot::setPlotTitle(QString plotTitle)
{
    if (mPlotTitle != plotTitle){
        ((QCPTextElement*)customplot->plotLayout()->element(0, 0))->setText(plotTitle);
        mPlotTitle = plotTitle;
    }
}

QString FormPlot::plotTitle() const
{
    return mPlotTitle;
}


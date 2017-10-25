#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DataParser.h"

#include <QMdiSubWindow>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mTimer.setInterval(5000);
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(on_timeout()));
    mTimer.start();

    connect(&DataParser::Instance(), SIGNAL(newName(QString)), this, SLOT(on_newName(QString)));
    connect(&DataParser::Instance(), SIGNAL(delName(QString)), this, SLOT(on_delName(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_newName(QString name)
{
    PagePlotSingleCurve* pCurve = new PagePlotSingleCurve;
    pCurve->setObjectName("Curve-" + name);

    QMdiSubWindow* pWindow = ui->mdiArea->addSubWindow(pCurve);
    pWindow->setObjectName(name);
    pWindow->showMaximized();
    pWindow->setWindowTitle(name);

    DataParser::Instance().registerObserver(name, pCurve);
}

void MainWindow::on_delName(QString name)
{
    QMdiSubWindow* pWindow = ui->mdiArea->findChild<QMdiSubWindow*>(name);
    if (pWindow){
        ui->mdiArea->removeSubWindow(pWindow);
        delete pWindow;
    }
}

void MainWindow::on_timeout()
{
    QStringList sl = DataParser::Instance().names();
    foreach(QString name, sl){
        QMdiSubWindow* pWindow = ui->mdiArea->findChild<QMdiSubWindow*>(name);
        if (!pWindow){
            on_newName(name);
        }
    }
}

void MainWindow::on_actionTile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionSave_triggered()
{
    QMdiSubWindow* pWindow = ui->mdiArea->activeSubWindow();
    if (pWindow){
        QString fileName = QFileDialog::getSaveFileName(this, "保存", "请选择要保存的文件文件名");
        if (!fileName.isEmpty()){
            if (!fileName.endsWith(".png")){
                fileName += ".png";
//                QPixmap pixmap = pWindow->gra();
//                if (!pixmap.save(fileName, "PNG"))
//                    QMessageBox::warning(this, "警告", QString("保存图片文件失败!"));
            }
        }
    }
}

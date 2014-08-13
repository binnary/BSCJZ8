#include "mainwindow.h"
#include "qwt_plot_renderer.h"
#include "disassaydata.h"
#include "deviceinfo.h"
#include "ui_mainwindow.h"
#include "QDockWidget"
#include "qcapture.h"
#include "qlog.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDockWidget *dock = new QDockWidget(this);
    dock->setFeatures (QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    mainAct = new QAction(QIcon(":/images/config.png"), tr("&Config"), this);
    mainAct->setStatusTip(tr("Export pdf file"));
    connect(mainAct, SIGNAL(triggered()), this, SLOT(Capture()));
    ui->mainToolBar->addAction(mainAct);

    disdataAct = new QAction(QIcon(":/images/player_pause.png"), tr("&Pause"), this);
    disdataAct->setStatusTip(tr("Display db info"));
    connect(disdataAct, SIGNAL(triggered()), this, SLOT(displayData()));
    ui->mainToolBar->addAction(disdataAct);

    disitemsAct = new QAction(QIcon(":/images/config.png"), tr("&Config"), this);
    disitemsAct->setStatusTip(tr("Display db eee"));
    connect(disitemsAct, SIGNAL(triggered()), this, SLOT(displayItems()));
    ui->mainToolBar->addAction(disitemsAct);
    disActGraphic = new QAction(QIcon(":/images/exit.png"), tr("&Graphic"), this);
    disActGraphic->setStatusTip(tr("Display Data Graphic Mode"));
    connect(disActGraphic, SIGNAL(triggered()), this, SLOT(displayGraphic()));
    ui->mainToolBar->addAction(disActGraphic);

    printAct = new QAction(QIcon(":/images/fileprint.png"), tr("&Print"), this);
    printAct->setStatusTip(tr("Display Data Graphic Mode"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(printFunc()));
    ui->mainToolBar->addAction(printAct);

    printPdfAct = new QAction(QIcon(":/images/exportpdf.png"), tr("&ExportPDF"), this);
    printPdfAct->setStatusTip(tr("Export pdf file"));
    connect(printPdfAct, SIGNAL(triggered()), this, SLOT(exportpdf()));
    ui->mainToolBar->addAction(printPdfAct);

    QCapture *capture = new QCapture();
    setCentralWidget (capture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Capture()
{
    QCapture *capture = new QCapture();
    setCentralWidget (capture);
}
void MainWindow::displayData()
{
  //Display *p_mdisp = new Display;
  //p_mdisp->ResetTable ("AssayData");
  //setCentralWidget (p_mdisp);
  DisAssayData *mDevInfo = new DisAssayData();
  setCentralWidget (mDevInfo);
}

void MainWindow::displayItems()
{
  DeviceInfo *mDevInfo = new DeviceInfo();
  setCentralWidget (mDevInfo);
}

void MainWindow::displayGraphic()
{
  QDispgraphic *p_mDispGraphic = new QDispgraphic;
  setCentralWidget (p_mDispGraphic);
}

void MainWindow::printFunc()
{
    QPrinter printer( QPrinter::HighResolution );

    QString docName = this->windowTitle ();
    if ( !docName.isEmpty() )
    {
        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
        printer.setDocName ( docName );
    }

    printer.setCreator( "Bode example" );
    printer.setOrientation( QPrinter::Landscape );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

//        renderer.renderTo(this, printer );
    }

}
void MainWindow::exportpdf()
{

}

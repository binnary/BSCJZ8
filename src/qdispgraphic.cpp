#include "qdispgraphic.h"
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_zoomer.h>
#include "ui_qdispgraphic.h"
#include <QPointF>
#include <QVector>
#include <QSqlRecord>
#include <QSqlQuery>
#include  <QPrinter>
#include <QPrintDialog>
#include <qwt_plot_renderer.h>
QDispgraphic::QDispgraphic(QWidget *parent) :
     QWidget(parent),
    ui(new Ui::QDispgraphic)
{
    ui->setupUi(this);
    mModel = new QSqlTableModel();
    mModel->setTable("AssayData");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();
    InitPlot ();
    InitCurve("Ch4", Qt::red);
    queryData("Ch4");

    QSqlQuery query;
    query.exec("SELECT * from AssayData");
    while(query.next ()){
        QString value = query.value("DeviceId").toString ();
        if (ui->comboBox_DevIndex->findText (value) < 0 ){
            ui->comboBox_DevIndex->addItem (value);
        }
        value = query.value("PipeId").toString ();
        if (ui->comboBox_PipeIndexEnd->findText (value) < 0 ){
            ui->comboBox_PipeIndexEnd->addItem (value);
        }
        if (ui->comboBox_PipeIndexStart->findText(value) < 0 ){
            ui->comboBox_PipeIndexStart->addItem (value);
        }
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(DisplayChange(QString)),Qt::QueuedConnection);

    connect(ui->comboBox_DevIndex, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(DevIdChange(QString)),Qt::QueuedConnection);
    connect(ui->pushButton_Print, SIGNAL(clicked()), this, SLOT(printFunc()));
}
QDispgraphic::~QDispgraphic()
{
    delete ui;
    QList<DispCurve*>::iterator it;
    for (it=mCurveList.begin (); it!=mCurveList.end (); it++){
        delete *it;
    }

}

void QDispgraphic::InitPlot()
{
    ui->qwtPlot->setAutoFillBackground( true );
    ui->qwtPlot->setPalette( palette ());
    ui->qwtPlot->setAutoReplot( true );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 0 );
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setBorderRadius( 15 );

//    QPalette canvasPalette( Qt::white );
//    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
//    canvasPalette.setColor( palette ());
    canvas->setPalette( palette ());
    ui->qwtPlot->setCanvas( canvas );
    // panning with the left mouse button
  ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
//    ( void ) new QwtPlotMagnifier( canvas );
    ( void ) new DisPlotMagnifier( canvas );



    ui->qwtPlot->setAxisTitle( QwtPlot::xBottom, "X-->" );
    ui->qwtPlot->setAxisScale( QwtPlot::xBottom, 0,20);
    ui->qwtPlot->setAxisScale( QwtPlot::xBottom, 0, mModel->rowCount ());
    ui->qwtPlot->setAxisLabelRotation( QwtPlot::xBottom, -50.0 );
    ui->qwtPlot->setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

    ui->qwtPlot->setAxisTitle( QwtPlot::yLeft, "Y-->" );
    ui->qwtPlot->setAxisScale( QwtPlot::yLeft, 0, 1000);
//    QwtPlotZoomer* zoomer = new QwtPlotZoomer( ui->qwtPlot->canvas() );
//    zoomer->setRubberBandPen( QColor( Qt::black ) );
//    zoomer->setTrackerPen( QColor( Qt::black ) );
//    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier );
//    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton );
//    QwtLegend *legend = new QwtLegend;
//    legend->setDefaultItemMode( QwtLegendData::Checkable );
//    ui->qwtPlot->insertLegend( legend, QwtPlot::RightLegend );
//    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
//        SLOT( legendChecked( const QVariant &, bool ) ) );
}
void QDispgraphic::printFunc()
{
    QPrinter printer( QPrinter::HighResolution );

    QString docName = ui->qwtPlot->title().text();
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

        renderer.renderTo(ui->qwtPlot, printer );
    }
}
void QDispgraphic::exportpdf()
{

}

void QDispgraphic::queryData(QString name)
{
    QVector<QPointF> temp;
    int maxY = 0;
    int Y;
    int count = 0;
    while((count++) < mModel->rowCount ()){
         Y = mModel->record (count).value (name).toInt ();
         temp.push_back (QPointF(count, Y));
         Y >= maxY ? maxY = Y : maxY = maxY;
    }
    ui->qwtPlot->setAxisScale( QwtPlot::xBottom, 0, count);
    ui->qwtPlot->setAxisScale( QwtPlot::yLeft, 0, maxY);
    QList<DispCurve*>::iterator it;
    for (it=mCurveList.begin (); it!=mCurveList.end (); it++){
        if (((DispCurve*)*it)->title().text() == name){
            ((DispCurve*)*it)->setSamples (temp);
        }

    }
    ui->qwtPlot->replot();
}
void QDispgraphic::DisplayChange(QString index)
{
    qDebug() << index;
    queryData (index);
}
void QDispgraphic::DevIdChange(QString index)
{
    qDebug() << index;
    QString filter("DeviceId= ");
    filter += index;
    mModel->setFilter (filter);
    mModel->select ();
    queryData (ui->comboBox->currentText ());
}

void QDispgraphic::queryData(QString X, QString Y)
{
   QVector<QPointF> temp;
   int count = 1;
   while((count++) < mModel->rowCount ()){
        temp.push_back (QPointF(mModel->record (count).value (X).toInt (),
                                mModel->record (count).value (Y).toInt ()));
   }
   ui->qwtPlot->replot ();
}
void QDispgraphic::legendChecked( const QVariant &itemInfo, bool on )
{
    QwtPlotItem *plotItem = ui->qwtPlot->infoToItem( itemInfo );
    if ( plotItem )
        showCurve( plotItem, on );
}

void QDispgraphic::showCurve( QwtPlotItem *item, bool on )
{
    item->setVisible( on );

    QwtLegend *lgd = qobject_cast<QwtLegend *>( ui->qwtPlot->legend() );

    QList<QWidget *> legendWidgets =
        lgd->legendWidgets( ui->qwtPlot->itemToInfo( item ) );

    if ( legendWidgets.size() == 1 )
    {
        QwtLegendLabel *legendLabel =
            qobject_cast<QwtLegendLabel *>( legendWidgets[0] );

        if ( legendLabel )
            legendLabel->setChecked( on );
    }

    ui->qwtPlot->replot();
}
void QDispgraphic::InitCurve(QString name, QColor color)
{
    DispCurve *curve = NULL;
    curve = new DispCurve(name);
    curve->setPen(color);
    curve->setZ( curve->z() - 2 );
    curve->attach(ui->qwtPlot);
    mCurveList.push_back (curve);
}

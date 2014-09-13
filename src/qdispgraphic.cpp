#include "qdispgraphic.h"
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_picker_machine.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <qwt_legend_label.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_zoomer.h>
#include "paramlevelconfig.h"
#include "ui_qdispgraphic.h"
#include "dbopt.h"
#include <QPointF>
#include <QVector>
#include <QSqlRecord>
#include <QSqlQuery>
#include  <QPrinter>
#include <QPrintDialog>
#include <qwt_plot_renderer.h>
QDispgraphic::QDispgraphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QDispgraphic),
    mMarkerMax(NULL),
    mMarkerMin(NULL),
    mPicker(NULL),
    mGrid(NULL)
{
    ui->setupUi(this);
    mModel = new QSqlTableModel();
    mModel->setTable("AssayData");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();
    InitPlot ();
    InitCurve("Ch4", Qt::black);
    queryData("Ch4");
//    queryData("AssayTime","Ch4");

    QSqlQuery query;
    query.exec("SELECT * from AssayData");
    while(query.next ()) {
        QString value = query.value("DeviceId").toString ();
        if (ui->comboBox_DevIndex->findText (value) < 0 ) {
            ui->comboBox_DevIndex->addItem (value);
        }
        value = query.value("PipeId").toString ();
        if (ui->comboBox_PipeIndexEnd->findText (value) < 0 ) {
            ui->comboBox_PipeIndexEnd->addItem (value);
        }
        if (ui->comboBox_PipeIndexStart->findText(value) < 0 ) {
            ui->comboBox_PipeIndexStart->addItem (value);
        }
    }
    query.exec("SELECT MAX(AssayTime), MIN(AssayTime) FROM AssayData");
    if(query.next ()) {
        ui->dateTimeEdit_Start->setDateTime(
            QDateTime::fromString (
                query.value ("MIN(AssayTime)").toString (),
                QString("yyyy/M/d/hh:mm:ss"))
        );
        ui->dateTimeEdit_End->setDateTime(
            QDateTime::fromString (
                query.value ("MAX(AssayTime)").toString (),
                QString("yyyy/M/d/hh:mm:ss"))
        );
    }
    query.exec("SELECT * from FriendlyName");
    while(query.next ()){
        qDebug() << query.value ("RangeMax").toString ().size ();
        qDebug() <<query.value ("RangeMin").toString ().size ();
        if (query.value ("RangeMax").toString ().size ()){
            ui->comboBox->addItem (query.value ("FriendlyName").toString ());
        }
    }
    DevIdChange (ui->comboBox_DevIndex->currentText ());
    connect(ui->checkBox_DevIndex,
            SIGNAL(clicked()),
            this,
            SLOT(FilterQuery()));
    connect(ui->comboBox_DevIndex,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(DevIdChange(QString)));
    connect(ui->checkBox_StartEndDate,
            SIGNAL(clicked()),
            this,
            SLOT(FilterQuery()));
    connect(ui->dateTimeEdit_End,
            SIGNAL(dateTimeChanged(QDateTime)),
            this,
            SLOT(FilterQuery()));
    connect(ui->dateTimeEdit_Start,
            SIGNAL(dateTimeChanged(QDateTime)),
            this,
            SLOT(FilterQuery()));
    connect(ui->checkBox_PipeIndex,
            SIGNAL(clicked()),
            this,
            SLOT(FilterQuery()));
    connect(ui->comboBox_PipeIndexStart,
            SIGNAL(currentTextChanged(QString)),
            this,
            SLOT(FilterQuery()));
    connect(ui->comboBox_PipeIndexEnd,
            SIGNAL(currentTextChanged(QString)),
            this,
            SLOT(FilterQuery()));
    connect(ui->comboBox,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(DisplayChange(QString)),
            Qt::QueuedConnection);
    connect(ui->pushButton_print,
            SIGNAL(clicked()),
            this,
            SLOT(printFunc()));
    connect(ui->spinBox_Max,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(MaxMarkervalueChanged(double)));
    connect(ui->spinBox_Min,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(MinMarkervalueChanged(double)));
    connect(ui->pushButton_Config,
            SIGNAL(clicked()),
            this,
            SLOT(ConfigParamLevel()));
}
QDispgraphic::~QDispgraphic()
{
    delete ui;
    QList<DispCurve*>::iterator it;
    for (it=mCurveList.begin (); it!=mCurveList.end (); it++) {
        delete *it;
    }
    if(mMarkerMax) {
        delete mMarkerMax;
    }
    if(mMarkerMin) {
        delete mMarkerMin;
    }
    if(mPicker) {
        delete mPicker;
    }
    if(mGrid) {
        delete mGrid;
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
    canvas->setBorderRadius( 10 );
//    canvas->setLegendAttribute( QwtPlotCurve::LegendShowLine );

//    QPalette canvasPalette( Qt::gray);
//    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
//    canvasPalette.setColor( palette ());
//    canvas->setPalette(canvasPalette);
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
    ui->qwtPlot->setAxisLabelRotation( QwtPlot::xBottom, -20.0 );
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
    // grid
    mGrid = new QwtPlotGrid();
    mGrid->enableXMin( true );
    mGrid->setMajorPen( Qt::darkGray, 2, Qt::DotLine );
    mGrid->setMinorPen( Qt::lightGray, 2 , Qt::DotLine );
    mGrid->attach(ui->qwtPlot);
    // marker
    mMarkerMax = new QwtPlotMarker();
    mMarkerMax->setValue( 0.0, 0.0 );
    QwtText Lable("Max 0.0");
    mMarkerMax->setLabel (Lable);
    mMarkerMax->setLineStyle( QwtPlotMarker::HLine );
    mMarkerMax->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    mMarkerMax->setLinePen( Qt::red, 1, Qt::SolidLine );
    mMarkerMax->attach(ui->qwtPlot);
    // marker
    mMarkerMin = new QwtPlotMarker();
    QwtText minLable("Min 0.0");
    mMarkerMin->setLabel (minLable);
    mMarkerMin->setValue( 0.0, 0.0 );
    mMarkerMin->setLineStyle( QwtPlotMarker::HLine );
    mMarkerMin->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    mMarkerMin->setLinePen( Qt::darkYellow, 1, Qt::SolidLine );
    mMarkerMin->attach(ui->qwtPlot);
    mPicker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        ui->qwtPlot->canvas ());
    mPicker->setStateMachine( new QwtPickerDragPointMachine() );
    mPicker->setRubberBandPen( QColor( Qt::green ) );
    mPicker->setRubberBand( QwtPicker::CrossRubberBand );
    mPicker->setTrackerPen( QColor( Qt::darkBlue ) );
}

void QDispgraphic::MaxMarkervalueChanged(double value)
{
    if(!mMarkerMax){
        return;
    }
    mMarkerMax->setValue( 0.0, value);
    //QString Lable = QObject::tr("Range(");
    //Lable += QString::number (minY)+ QString(",") +QString::number (maxY) +QString(")");
    //mMarkerMax->setLabel (QwtText(Lable));
}
void QDispgraphic::MinMarkervalueChanged(double value)
{
    if(!mMarkerMin){
        return;
    }
    mMarkerMin->setValue( 0.0, value);
}
void QDispgraphic::ConfigParamLevel()
{
   ParamLevelConfig  config;
   config.exec ();
}
void QDispgraphic::printFunc()
{
    QPrinter printer( QPrinter::HighResolution );

    QString docName = ui->qwtPlot->title().text();
    if ( !docName.isEmpty() ) {
        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
        printer.setDocName ( docName );
    }

    printer.setCreator( "Platform");
    printer.setOrientation( QPrinter::Landscape );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() ) {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale ) {
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
    int minY = 0;
    int Y;
    int count = 0;
    while((count++) < mModel->rowCount ()) {
        Y = mModel->record (count).value (name).toInt ();
        temp.push_back (QPointF(count, Y));
        Y >= maxY ? maxY = Y : maxY = maxY;
        Y <= minY ? minY = Y : minY = minY;
    }
    //mMarkerMax->setValue( 0.0, maxY);
    //QString Lable = QObject::tr("Range(");
    //Lable += QString::number (minY)+ QString(",") +QString::number (maxY) +QString(")");
    //mMarkerMax->setLabel (QwtText(Lable));
    //mMarkerMax->setLabelAlignment(Qt::AlignLeft);

    //mMarkerMin->setValue( 0.0, minY);
    //Lable.setText (QString("Min ") + QString::number (minY));
    //mMarkerMin->setLabel (Lable);

    ui->qwtPlot->setAxisScale( QwtPlot::xBottom, 0, count);
    ui->qwtPlot->setAxisScale( QwtPlot::yLeft, 0, maxY*12/10);
    QList<DispCurve*>::iterator it;
    for (it=mCurveList.begin (); it!=mCurveList.end (); it++) {
        if (((DispCurve*)*it)->title().text() == name) {
            ((DispCurve*)*it)->setSamples (temp);
        }

    }
    ui->qwtPlot->replot();
}
void QDispgraphic::DisplayChange(QString index)
{
    QString sql = QString("SELECT * FROM FriendlyName Where Name='")+
               GetTableColumByFriendName(index) + QString("'");
    QSqlQuery query;
    query.exec(sql);
    if (query.next ()){
        ui->spinBox_Max->setValue (query.value ("RangeMax").toDouble ());
        MaxMarkervalueChanged (query.value ("RangeMax").toDouble ());
        ui->spinBox_Min->setValue (query.value ("RangeMin").toDouble ());
        MinMarkervalueChanged (query.value ("RangeMin").toDouble ());
    }
    FilterQuery ();
}
void QDispgraphic::DevIdChange(QString index)
{
    qDebug() <<index;
//    qDebug() << index;
    //QString filter("DeviceId= ");
    //filter += index;
    //mModel->setFilter (filter);
    //mModel->select ();
    //queryData (ui->comboBox->currentText ());
    FilterQuery ();
}

void QDispgraphic::queryData(QString X, QString Y)
{
    QVector<QPointF> temp;
    int maxY = 0;
    int IntY;
    int count = 1;
    uint minAssayTime = 0;
    QSqlQuery query;
    query.exec("SELECT MAX(AssayTime), MIN(AssayTime) FROM AssayData");
    if(query.next ()) {
        minAssayTime = QDateTime::fromString (
                        query.value ("MIN(AssayTime)").toString (),"yyyy/M/d/hh:mm:ss")
                       .toTime_t ();
    }

    while((count++) < mModel->rowCount ()) {
        IntY = mModel->record (count).value (Y).toInt ();
        uint IntX =QDateTime::fromString (
                        mModel->record (count).value ("AssayTime").toString (),"yyyy/M/d/hh:mm:ss")
                       .toTime_t ();
                 qDebug()<< " IntX:" << IntX << " IntY:" << IntY;
        temp.push_back (QPointF(IntX-minAssayTime,IntY));
        IntY >= maxY ? maxY = IntY : maxY = maxY;
    }
    ui->qwtPlot->setAxisScale( QwtPlot::xBottom, 0, count);
    ui->qwtPlot->setAxisScale( QwtPlot::yLeft, 0, maxY);
    QList<DispCurve*>::iterator it;
    for (it=mCurveList.begin (); it!=mCurveList.end (); it++) {
        if (((DispCurve*)*it)->title().text() == "Ch4") {
            ((DispCurve*)*it)->setSamples (temp);
        }

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

    if ( legendWidgets.size() == 1 ) {
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
    curve->setStyle (DispCurve::Sticks);
//    curve->setCurveAttribute(DispCurve::Fitted);
    curve->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    curve->setSymbol( new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
        QPen( Qt::red ), QSize( 5, 5 ) ) );
    curve->setPen(color);
    curve->setZ( curve->z() - 2 );
    curve->attach(ui->qwtPlot);
    mCurveList.push_back (curve);
}
void QDispgraphic::FilterQuery()
{
    QString filter;
    if (ui->checkBox_DevIndex->isChecked ()) {
        filter = QString("DeviceId ='") + ui->comboBox_DevIndex->currentText ()+ "'";
    }
    //TODO filter by datetime
    if (ui->checkBox_StartEndDate->isChecked ()) {
        if(filter.size ()) {
            filter+= QString(" and ");
        }
        filter += QString ("AssayTime>='") +
                  ui->dateTimeEdit_Start->dateTime().toString ("yyyy/M/d/hh:mm:ss")+
                  QString("' and AssayTime<='") +
                  ui->dateTimeEdit_End->dateTime().toString ("yyyy/M/d/hh:mm:ss") +
                  QString("'");
    }
    if (ui->checkBox_PipeIndex->isChecked ()) {
        if(filter.size ()) {
            filter+= QString(" and ");
        }
        filter += QString("PipeID>='") +
                  ui->comboBox_PipeIndexStart->currentText ()+
                  "' and PipeID<='" +
                  ui->comboBox_PipeIndexEnd->currentText ()+
                  QString("'");
    }
    QString GraphicLable;
    GraphicLable =QString("X-->\n") + ui->comboBox->currentText () +QString(" Filter: ") +filter;

    ui->qwtPlot->setAxisTitle( QwtPlot::xBottom, GraphicLable );
    qDebug() <<  filter;
    mModel->setFilter(filter + QString(" ORDER BY AssayTime"));
    mModel->setFilter(filter);
    mModel->select ();
//    queryData("AssayTime","Ch4");
//    queryData ("AssayTime",ui->comboBox->currentText ());
    queryData (GetTableColumByFriendName(ui->comboBox->currentText ()));
}

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
#include <qwt_series_data.h>
#include <qwt_series_store.h>
#include <qwt_point_data.h>
#include "ui_qdispgraphic.h"
#include <QPointF>
#include <QVector>
#include <QSqlRecord>

QDispgraphicPrivate::QDispgraphicPrivate(QWidget *parent):
    QwtPlot(parent)
{
    setAutoFillBackground( true );
    setPalette( palette ());
    setAutoReplot( true );
    InitPlot();
}
QDispgraphicPrivate::~QDispgraphicPrivate ()
{
    QList<DispCurve*>::iterator it;
    for (it=mCurveList.begin (); it!=mCurveList.end (); it++){
        delete *it;
    }

}
void QDispgraphicPrivate::InitPlot()
{
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 0 );
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setBorderRadius( 15 );

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
//    canvasPalette.setColor( palette ());
    canvas->setPalette( palette ());
    setCanvas( canvas );
    // panning with the left mouse button
  ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier( canvas );

    setAxisTitle( QwtPlot::xBottom, "X-->" );
    setAxisScale( QwtPlot::xBottom, 0,20);
    setAxisScale( QwtPlot::xBottom, 0, 1024);
    setAxisLabelRotation( QwtPlot::xBottom, -50.0 );
    setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

    setAxisTitle( QwtPlot::yLeft, "Y-->" );
    setAxisScale( QwtPlot::yLeft, 0, 1);
//    QwtLegend *legend = new QwtLegend;
//    legend->setDefaultItemMode( QwtLegendData::Checkable );
//    ui->qwtPlot->insertLegend( legend, QwtPlot::RightLegend );
//    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
//        SLOT( legendChecked( const QVariant &, bool ) ) );
}
void QDispgraphicPrivate::legendChecked( const QVariant &itemInfo, bool on )
{
    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if ( plotItem )
        showCurve( plotItem, on );
}

void QDispgraphicPrivate::showCurve( QwtPlotItem *item, bool on )
{
    item->setVisible( on );

    QwtLegend *lgd = qobject_cast<QwtLegend *>( legend() );

    QList<QWidget *> legendWidgets =
        lgd->legendWidgets(itemToInfo( item ) );

    if ( legendWidgets.size() == 1 )
    {
        QwtLegendLabel *legendLabel =
            qobject_cast<QwtLegendLabel *>( legendWidgets[0] );

        if ( legendLabel )
            legendLabel->setChecked( on );
    }

    replot();
}
void QDispgraphicPrivate::InitCurve(QString name, QColor color)
{
    DispCurve *curve = NULL;
    curve = new DispCurve(name);
    curve->setPen(color);
    curve->setZ( curve->z() - 2 );
    curve->attach(this);
    mCurveList.push_back (curve);
}

QList<DispCurve*> &QDispgraphicPrivate::getCurves()
{
    return mCurveList;
}
QDispgraphic::QDispgraphic(QWidget *parent) :
     QWidget(parent)
{
    setupUi(this);
    mModel = new QSqlTableModel();
    mModel->setTable("AssayData");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();

    qwtPlot->InitCurve ("Ch4", Qt::red);
    queryData("Ch4");
}

QDispgraphic::~QDispgraphic()
{
}

void QDispgraphic::queryData(QString name)
{
    QVector<QPointF> temp;
//    QwtArraySeriesData<QPointF> temp1;
//    QwtSyntheticPointData *temp1 = new QwtSyntheticPointData(1000);
//    QwtSeriesData<QPointF> temp1;
//    QwtPointSeriesData QwtArraySeriesData<QPointF> temp1;
    int count = 0;
    int maxY= 0;
    int value = 0;
    while((count++) < mModel->rowCount ()){
         value =  mModel->record (count).value (name).toInt ();
         (value >= maxY) ? maxY=value : maxY=maxY;
         temp.push_back (QPointF(count, value));
    }
    qwtPlot->setAxisScale( QwtPlot::xBottom, 0, count);
    qwtPlot->setAxisScale( QwtPlot::yLeft, 0, maxY);
    QList<DispCurve*>  curve = qwtPlot->getCurves();
    QList<DispCurve*>::iterator it;
    for (it=curve.begin (); it!=curve.end (); it++){
        if (((DispCurve*)*it)->title().text() == name){
//            ((DispCurve*)*it)->setSamples (temp);
//            temp1.setSamples (temp);
            ((DispCurve*)*it)->setSamples (temp);
        }
    }

}
void QDispgraphic::queryData(QString X, QString Y)
{
   QVector<QPointF> temp;
   int count = 1;
   while((count++) < mModel->rowCount ()){
        temp.push_back (QPointF(mModel->record (count).value (X).toInt (),
                                mModel->record (count).value (Y).toInt ()));
   }
   qwtPlot->replot ();
}
void QDispgraphic::printFunc()
{
//    QPrinter printer( QPrinter::HighResolution );

//    QString docName = this->windowTitle ();
//    if ( !docName.isEmpty() )
//    {
//        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
//        printer.setDocName ( docName );
//    }

//    printer.setCreator( "Bode example" );
//    printer.setOrientation( QPrinter::Landscape );

//    QPrintDialog dialog( &printer );
//    if ( dialog.exec() )
//    {
//        QwtPlotRenderer renderer;

//        if ( printer.colorMode() == QPrinter::GrayScale )
//        {
//            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
//            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
//            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
//            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
//        }

//        renderer.renderTo(this, printer );
//    }

}
void QDispgraphic::exportpdf()
{

}

void QDispgraphic::setupUi(QWidget *QDispgraphic)
{
    if (QDispgraphic->objectName().isEmpty())
        QDispgraphic->setObjectName(QStringLiteral("QDispgraphic"));
    QDispgraphic->resize(729, 537);
    gridLayout = new QGridLayout(QDispgraphic);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    comboBox = new QComboBox(QDispgraphic);
    comboBox->setObjectName(QStringLiteral("comboBox"));

    horizontalLayout->addWidget(comboBox);

    pushButton_disp = new QPushButton(QDispgraphic);
    pushButton_disp->setObjectName(QStringLiteral("pushButton"));

    horizontalLayout->addWidget(pushButton_disp);

    pushButton_Print = new QPushButton(QDispgraphic);
    pushButton_Print->setObjectName(QStringLiteral("pushButton_Print"));
    pushButton_Print->setIconSize(QSize(16, 16));

    horizontalLayout->addWidget(pushButton_Print);

    pushButton_Expdf = new QPushButton(QDispgraphic);
    pushButton_Expdf->setObjectName(QStringLiteral("pushButton_2"));

    horizontalLayout->addWidget(pushButton_Expdf);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);


    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

    line = new QFrame(QDispgraphic);
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(line, 1, 0, 1, 1);

    qwtPlot = new QDispgraphicPrivate(QDispgraphic);
    qwtPlot->setObjectName(QStringLiteral("qwtPlot"));
    qwtPlot->setAutoFillBackground(true);
    QBrush brush(QColor(212, 208, 200, 255));
    brush.setStyle(Qt::Dense6Pattern);
    qwtPlot->setCanvasBackground(brush);
    qwtPlot->setAutoReplot(true);

    gridLayout->addWidget(qwtPlot, 2, 0, 1, 1);


    retranslateUi(QDispgraphic);

    QMetaObject::connectSlotsByName(QDispgraphic);
} // setupUi

void QDispgraphic::retranslateUi(QWidget *QDispgraphic)
{
    QDispgraphic->setWindowTitle(QApplication::translate("QDispgraphic", "Form", 0));
    comboBox->clear();
    comboBox->insertItems(0, QStringList()
     << QApplication::translate("QDispgraphic", "Flux", 0)
     << QApplication::translate("QDispgraphic", "Ch4", 0)
     << QApplication::translate("QDispgraphic", "Pressure", 0)
     << QApplication::translate("QDispgraphic", "Temperature", 0)
     << QApplication::translate("QDispgraphic", "CO", 0)
     << QApplication::translate("QDispgraphic", "CO2", 0)
     << QApplication::translate("QDispgraphic", "O2", 0)
    );
    pushButton_disp->setText(QApplication::translate("QDispgraphic", "Display", 0));
    pushButton_Print->setText(QApplication::translate("QDispgraphic", "Print", 0));
    pushButton_Expdf->setText(QApplication::translate("QDispgraphic", "Export Pdf", 0));
} // retranslateUi

#ifndef QDISPGRAPHIC_H
#define QDISPGRAPHIC_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_zoneitem.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_div.h>
#include <qwt_plot_grid.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_curve.h>
#include <qwt_date.h>
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

#include <QList>
#include <QSqlTableModel>
#include <QDebug>

namespace Ui
{
class QDispgraphic;
}

class  Picker: public QwtPlotPicker
{
public:
    Picker(QWidget *canvas ):
        QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                      QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas )
    {
        setRubberBandPen( QColor( Qt::darkGreen ) );
        setStateMachine( new QwtPickerDragPointMachine() );
        setRubberBandPen( QColor( Qt::green ) );
        setTrackerPen( QColor( Qt::darkBlue ) );
    }
protected:
    virtual QwtText trackerTextF( const QPointF &pos ) const
    {

        QString s;
        s = QwtDate::toString( QwtDate::toDateTime( pos.x() ),
                               "yyyy/MM/dd/hh:mm:ss ", QwtDate::FirstThursday );

        QwtText text( s );
        text.setColor( Qt::white );

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );

        return text;
    }
};

class DateScaleDraw: public QwtDateScaleDraw
{
public:
    DateScaleDraw( Qt::TimeSpec timeSpec ):
        QwtDateScaleDraw( timeSpec )
    {
        setDateFormat( QwtDate::Second, "hh:mm:ss\nddd dd MMM" );
        setDateFormat( QwtDate::Minute, "hh:mm\nddd dd MMM" );
        setDateFormat( QwtDate::Hour, "hh:mm\nddd dd MMM" );
        setDateFormat( QwtDate::Day, "ddd dd MMM" );
        setDateFormat( QwtDate::Week, "Www" );
        setDateFormat( QwtDate::Month, "MMM" );
    }
};

class DateScaleEngine: public QwtDateScaleEngine
{
public:
    DateScaleEngine( Qt::TimeSpec timeSpec ):QwtDateScaleEngine(timeSpec)
    {

    }
    ~DateScaleEngine()
    {

    }
    QwtDate::IntervalType intervalType(
        const QDateTime &, const QDateTime &, int) const
    {
        return QwtDate::Second;
    }

};
class ZoneItem: public QwtPlotZoneItem
{
public:
    ZoneItem( const QString &title )
    {
        setTitle( title );
        setZ( 11 ); // on top the the grid
        setOrientation( Qt::Vertical );
        setItemAttribute( QwtPlotItem::Legend, true );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;

        c.setAlpha( 100 );
        setPen( c );

        c.setAlpha( 20 );
        setBrush( c );
    }

    void setInterval( const QDate &date1, const QDate &date2 )
    {
        const QDateTime dt1( date1, QTime(), Qt::UTC );
        const QDateTime dt2( date2, QTime(), Qt::UTC );

        QwtPlotZoneItem::setInterval( QwtDate::toDouble( dt1 ),
                                      QwtDate::toDouble( dt2 ) );
    }
};

class DisPlotMagnifier: public QwtPlotMagnifier
{
public:
    explicit DisPlotMagnifier( QWidget *parent ):
        QwtPlotMagnifier(parent),
        mMin(0.0),
        mMax(100.0)
    {

    }
    virtual ~DisPlotMagnifier()
    {

    }

    void setAxisScale(double min, double max)
    {
        mMin =min;
        mMax = max;
    }
    virtual void widgetWheelEvent( QWheelEvent *event )
    {
        QwtPlotMagnifier::widgetWheelEvent (event);
        plot ()->setAxisScale (QwtPlot::yLeft,mMin, mMax);
    }
private:
    double mMin,mMax;
};
class DispCurve: public QwtPlotCurve
{
public:
    DispCurve( const QString &title ):
        QwtPlotCurve( title )
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;
        c.setAlpha( 150 );

        setPen( c );
        setBrush( c );
    }
};

class QDispgraphic : public QWidget
{
    Q_OBJECT

public:
    explicit QDispgraphic(QWidget *parent = 0);
    void showCurve( QwtPlotItem *item, bool on );
    ~QDispgraphic();
private:
    void queryData(QString name);
    void InitCurve(QString name, QColor color);
    void InitPlot();
public slots:
    void FilterQuery();
    void printFunc();
    void DisplayChange(QString index);
    void legendChecked( const QVariant &itemInfo, bool on );
    void MaxMarkervalueChanged(double value);
    void MinMarkervalueChanged(double value);
    void ConfigParamLevel();
private:
    QwtScaleDiv SecondScaleDiv() const;
    QwtScaleDiv yearScaleDiv() const;
private:
    Ui::QDispgraphic *ui;
    QList<DispCurve*> mCurveList;
    QSqlTableModel *mModel;
    QwtPlotMarker *mMarkerMax;
    QwtPlotMarker *mMarkerMin;
    Picker *mPicker;
    QwtPlotGrid *mGrid;
    QwtDateScaleDraw *mScaleDraw;
    QwtDateScaleEngine *mScaleEngine;
    DisPlotMagnifier *mDisPlot;
    QwtPlotPanner  *mPanner;
};

#endif // QDISPGRAPHIC_H

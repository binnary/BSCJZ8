#ifndef QDISPGRAPHIC_H
#define QDISPGRAPHIC_H

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <qwt_plot_canvas.h>
#include <QList>
#include <QSqlTableModel>

namespace Ui {
class QDispgraphic;
}

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
    void queryData(QString X, QString Y);
    void queryData(QString name);
    void InitCurve(QString name, QColor color);
    void InitPlot();
public slots:
    void printFunc();
    void exportpdf();
    void legendChecked( const QVariant &itemInfo, bool on );
private:
    Ui::QDispgraphic *ui;
    QList<DispCurve*> mCurveList;
    QSqlTableModel *mModel;
};

#endif // QDISPGRAPHIC_H

#ifndef QDISPGRAPHIC_H
#define QDISPGRAPHIC_H

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <qwt_plot_canvas.h>
#include <QList>
#include <QSqlTableModel>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

//namespace Ui {
//class QDispgraphic;
//}

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

class QDispgraphicPrivate : public QwtPlot
{
     Q_OBJECT
protected:
    virtual void resizeEvent( QResizeEvent *event ){
        QwtPlot::resizeEvent (event);
//        QPalette pal = palette();

//        const QColor buttonColor = pal.color( QPalette::Button );

//        QLinearGradient gradient( rect().topLeft(), rect().bottomLeft() );
//        gradient.setColorAt( 0.0, Qt::white );
//        gradient.setColorAt( 0.7, buttonColor );
//        gradient.setColorAt( 1.0, buttonColor );

//        pal.setBrush( QPalette::Window, gradient );
//        setPalette( pal );
//        replot ();
    }
public:
    explicit QDispgraphicPrivate(QWidget *parent = 0);
    void showCurve( QwtPlotItem *item, bool on );
    ~QDispgraphicPrivate();
    QList<DispCurve*> &getCurves();
    void InitCurve(QString name, QColor color);
    void InitPlot();
public slots:
    void legendChecked( const QVariant &itemInfo, bool on );
private:
    QList<DispCurve*> mCurveList;
};

class QDispgraphic : public QWidget
{
     Q_OBJECT
public:
    explicit QDispgraphic(QWidget *parent = 0);
    ~QDispgraphic();
private:
    void queryData(QString X, QString Y);
    void queryData(QString name);
public slots:
    void printFunc();
    void exportpdf();
private:
    void setupUi(QWidget *QDispgraphic);
    void retranslateUi(QWidget *QDispgraphic);
private:
    QSqlTableModel *mModel;
    QDispgraphicPrivate *qwtPlot;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QComboBox *comboBox;
    QPushButton *pushButton_disp;
    QPushButton *pushButton_Print;
    QPushButton *pushButton_Expdf;
    QSpacerItem *horizontalSpacer;
    QFrame *line;

};

#endif // QDISPGRAPHIC_H

#ifndef QPRINTOBJECT_H
#define QPRINTOBJECT_H

#include <QObject>
#include <QPrinter>
#include <QDebug>
#include <QStringList>
#include <QString>
class QTextDocument;
class QWidget;

class QPrintObject : public QObject
{
    Q_OBJECT
public:
    explicit QPrintObject(QObject *parent=0);
    ~QPrintObject();

    void print(const QTextDocument *document,
               const QString &caption=QString());
    /// Export the document to PDF
    void preview(const QTextDocument *document,
                 const QString &caption=QString());

    /// Get page size
    QPrinter::PageSize pageSize() const;
    /// Set page size
    void setPageSize(QPrinter::PageSize size);
    /// Get page orientation
    QPrinter::Orientation orientation() const;
    /// Set page orientation
    void setOrientation(QPrinter::Orientation orientation);

    /// Get left margin width
    double leftMargin() const;
    /// Set left margin width
    void setLeftMargin(double margin);
    /// Get right margin width
    double rightMargin() const;
    /// Set right margin width
    void setRightMargin(double margin);
    /// Get top margin width
    double topMargin() const;
    /// Set top margin width
    void setTopMargin(double margin);
    /// Get bottom margin width
    double bottomMargin() const;
    /// Set bottom margin width
    void setBottomMargin(double margin);
    /// Set all margins
    void setMargins(double margin);

    /// Get spacing between content and header and footer
    double spacing() const;
    /// Set spacing between content and header and footer
    void setSpacing(double spacing);

    /// Get header size
    double headerSize() const;
    /// Set header size
    void setHeaderSize(double size);
    /// Set header rule size
    double headerRule() const;
    /// Get header rule size
    void setHeaderRule(double pointsize);
    /// Get header text
    const QString &headerText() const;
    /// Set header text
    void setHeaderText(const QString &text);

    /// Get footer size
    double footerSize() const;
    /// Set footer size
    void setFooterSize(double size);
    /// Get footer rule size
    double footerRule() const;
    /// Set footer rule size
    void setFooterRule(double pointsize);
    /// Get footer text
    const QString &footerText() const;
    /// Set footer text
    void setFooterText(const QString &text);

    /// Get date format
    const QString &dateFormat() const;
    /// Set date format
    void setDateFormat(const QString &format);

private:
    // not copyable
    QPrintObject(const QPrintObject&);
    QPrintObject &operator=(const QPrintObject&);

    // return paper rect
    QRectF paperRect(QPaintDevice *device);
    // return printable rects
    QRectF contentRect(QPaintDevice *device);
    QRectF headerRect(QPaintDevice *device);
    QRectF footerRect(QPaintDevice *device);
public slots:
    // paint specific page
    void paintPage(QPainter *painter,
                   QTextDocument *document,
                   int pagenum);
    void DocdrawContents(QRectF rect, QString text, QPainter *painter, bool IsHeader);
signals:
    void setPags(int min,int max);
    void setCurrentPagsNumber(int pag);
    void setLabelText(const QString &text);
    void TriggerdrawContents(QRectF rect, QString text, QPainter *painter, bool IsHeader);

private slots:
    // common print routine
//    void print(QPrinter *printer);
    void print(QPrinter *printer, QPainter *painter);

private:
    QWidget *mParent;
    QPrinter *mPrinter;

    QTextDocument *mTempDoc;
//    QTextDocument  *mFooterAndHeader;

    double mLeftMargin;
    double mRightMargin;
    double mTopMargin;
    double mBottomMargin;
    double mSpacing;

    double mHeaderSize;
    double mHeaderRule;
    QString mHeaderText;
    double mFooterSize;
    double mFooterRule;
    QString mFooterText;

    QString mDateFormat;
};


#endif // QPRINTOBJECT_H

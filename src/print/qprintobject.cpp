#include "qprintobject.h"
#include <QAbstractTextDocumentLayout>
#include <QDate>
#include <QFileDialog>
#include <QLocale>
#include <QPainter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextDocumentWriter>
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
#include <QPrintPreviewDialog>
#endif
static inline double mmToInches(double mm)
{
    return mm * 0.039370147;
}

QPrintObject::QPrintObject(QObject *parent)
    : QObject(parent), mParent(0),
      mPrinter(new QPrinter(QPrinter::HighResolution)), mTempDoc(0),
      mLeftMargin(15.0), mRightMargin(15.0), mTopMargin(15.0),
      mBottomMargin(15.0), mSpacing(5.0),
      mHeaderSize(0.0), mHeaderRule(0.5), mHeaderText(QString()),
      mFooterSize(0.0), mFooterRule(0.5), mFooterText(QString()),
      mDateFormat()
{
    if (parent) mParent = qobject_cast<QWidget*>(parent);

    mPrinter->setFullPage(true);
    mPrinter->setOrientation(QPrinter::Portrait);

    // for convenience, default to US_Letter for C/US/Canada
    // NOTE: bug in Qt, this value is not loaded by QPrintDialog
    switch (QLocale::system().country()) {
    case QLocale::AnyCountry:
    case QLocale::Canada:
    case QLocale::UnitedStates:
    case QLocale::UnitedStatesMinorOutlyingIslands:
        mPrinter->setPageSize(QPrinter::Letter);
        break;
    default:
        mPrinter->setPageSize(QPrinter::A4);
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// ~QPrintObject()
///////////////////////////////////////////////////////////////////////////////
/// Destroys the QPrintObject object.
///////////////////////////////////////////////////////////////////////////////

QPrintObject::~QPrintObject()
{
    delete mPrinter;
}

QPrinter::PageSize QPrintObject::pageSize() const
{
    return mPrinter->pageSize();
}

///////////////////////////////////////////////////////////////////////////////
// setPageSize()
///////////////////////////////////////////////////////////////////////////////
/// Set the page size. Size is defined by the QPrinter::PageSize enumeration.
/// By default, this is QPrinter::Letter in US and Canada locales, and
/// QPrinter::A4 in other locales.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setPageSize(QPrinter::PageSize size)
{
    mPrinter->setPageSize(size);
}

///////////////////////////////////////////////////////////////////////////////
// orientation()
///////////////////////////////////////////////////////////////////////////////
/// Return the page orientation. This is defined by the QPrinterOrientation
/// enumeration.
///////////////////////////////////////////////////////////////////////////////

QPrinter::Orientation QPrintObject::orientation() const
{
    return mPrinter->orientation();
}

///////////////////////////////////////////////////////////////////////////////
// setOrientation()
///////////////////////////////////////////////////////////////////////////////
/// Set the page orientation. Orientation is defined by the
/// QPrinter::Orientation enumeration, and can be QPrinter::Portrait (the
/// default) or QPrinter::Landscape.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setOrientation(QPrinter::Orientation orientation)
{
    mPrinter->setOrientation(orientation);
}

///////////////////////////////////////////////////////////////////////////////
// leftMargin()
///////////////////////////////////////////////////////////////////////////////
/// Return the left page margin width in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::leftMargin() const
{
    return mLeftMargin;
}

///////////////////////////////////////////////////////////////////////////////
// setLeftMargin()
///////////////////////////////////////////////////////////////////////////////
/// Set the left margin width in millimeters. The default value is 15mm.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setLeftMargin(double margin)
{
    if ((margin > 0) && (margin < mPrinter->paperRect().width() / 2)) {
        mLeftMargin = margin;
    } else {
        mLeftMargin = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// rightMargin()
///////////////////////////////////////////////////////////////////////////////
/// Return the right page margin width in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::rightMargin() const
{
    return mRightMargin;
}

///////////////////////////////////////////////////////////////////////////////
// setRightMargin()
///////////////////////////////////////////////////////////////////////////////
/// Set the right margin width in millimeters. The default value is 15mm.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setRightMargin(double margin)
{
    if ((margin > 0) && (margin < mPrinter->paperRect().width() / 2)) {
        mRightMargin = margin;
    } else {
        mRightMargin = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// topMargin()
///////////////////////////////////////////////////////////////////////////////
/// Return the top page margin size in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::topMargin() const
{
    return mTopMargin;
}

///////////////////////////////////////////////////////////////////////////////
// setTopMargin()
///////////////////////////////////////////////////////////////////////////////
/// Set the top margin size in millimeters. The default value is 15mm.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setTopMargin(double margin)
{
    if ((margin > 0) && (margin < mPrinter->paperRect().height() / 4)) {
        mTopMargin = margin;
    } else {
        mTopMargin = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// bottomMargin()
///////////////////////////////////////////////////////////////////////////////
/// Return the bottom page margin size in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::bottomMargin() const
{
    return mBottomMargin;
}

///////////////////////////////////////////////////////////////////////////////
// setBottomMargin()
///////////////////////////////////////////////////////////////////////////////
/// Set the bottom margin size in millimeters. The default value is 15mm.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setBottomMargin(double margin)
{
    if ((margin > 0) && (margin < mPrinter->paperRect().height() / 4)) {
        mBottomMargin = margin;
    } else {
        mBottomMargin = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// setMargins()
///////////////////////////////////////////////////////////////////////////////
/// Set all margins to have the identical size. The default value is 15mm.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setMargins(double margin)
{
    if ((margin > 0)
            && (margin < mPrinter->paperRect().height() / 2)
            && (margin < mPrinter->paperRect().width() / 2)) {
        mLeftMargin = mRightMargin = mTopMargin = mBottomMargin = margin;
    } else {
        mLeftMargin = mRightMargin = mTopMargin = mBottomMargin = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// spacing()
///////////////////////////////////////////////////////////////////////////////
/// Return the spacing between the page content and the header and footer
/// blocks. This is defined in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::spacing() const
{
    return mSpacing;
}

///////////////////////////////////////////////////////////////////////////////
// setSpacing()
///////////////////////////////////////////////////////////////////////////////
/// Set the spacing between the page content and the header and footer blocks.
/// The default value is 5mm.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setSpacing(double spacing)
{
    if ((spacing > 0) && (spacing <= mPrinter->paperRect().height() / 8)) {
        mSpacing = spacing;
    } else {
        mSpacing = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// headerSize()
///////////////////////////////////////////////////////////////////////////////
/// Return the height of the header block  in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::headerSize() const
{
    return mHeaderSize;
}

///////////////////////////////////////////////////////////////////////////////
// setHeaderSize()
///////////////////////////////////////////////////////////////////////////////
/// Set the height of the header in millimeters. The default is zero (no
/// header).
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setHeaderSize(double size)
{
    if ((size > 0) && (size <= mPrinter->paperRect().height() / 8)) {
        mHeaderSize = size;
    } else {
        mHeaderSize = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// headerRule()
///////////////////////////////////////////////////////////////////////////////
/// Return the size of the header rule in points. One point is 1/72 inch.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::headerRule() const
{
    return mHeaderRule;
}

///////////////////////////////////////////////////////////////////////////////
// setHeaderRule()
///////////////////////////////////////////////////////////////////////////////
/// Set the header rule size in points. By default, the header rule is one half
/// point (1/144 inch). To turn off the rule, set the rule size to 0. The rule
/// will be  drawn below the header area.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setHeaderRule(double pointsize)
{
    mHeaderRule = qMax(0.0, pointsize);
}

///////////////////////////////////////////////////////////////////////////////
// headerText()
///////////////////////////////////////////////////////////////////////////////
/// Return the text for the header.
///////////////////////////////////////////////////////////////////////////////

const QString &QPrintObject::headerText() const
{
    return mHeaderText;
}

///////////////////////////////////////////////////////////////////////////////
// setHeaderText()
///////////////////////////////////////////////////////////////////////////////
/// Set the text for the header. Rich text is supported. HTML tags may be used
/// to format the text and align elements. The following page variables may be
/// included in the text:
/// - \&page; - Insert current page number
/// - \&date; - Insert current date, using the format set with setPageFormat()
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setHeaderText(const QString &text)
{
    mHeaderText = text;
}

///////////////////////////////////////////////////////////////////////////////
// footerSize()
///////////////////////////////////////////////////////////////////////////////
/// Return the height of the footer block in millimeters.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::footerSize() const
{
    return mFooterSize;
}

///////////////////////////////////////////////////////////////////////////////
// setFooterSize()
///////////////////////////////////////////////////////////////////////////////
/// Set the height of the footer in millimeters. The default is zero (no
/// header).
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setFooterSize(double size)
{
    if ((size > 0) && (size <= mPrinter->paperRect().height() / 8)) {
        mFooterSize = size;
    } else {
        mFooterSize = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// footerRule()
///////////////////////////////////////////////////////////////////////////////
/// Return the size of the footer rule in points. One point is 1/72 inch.
///////////////////////////////////////////////////////////////////////////////

double QPrintObject::footerRule() const
{
    return mFooterRule;
}

///////////////////////////////////////////////////////////////////////////////
// setFooterRule()
///////////////////////////////////////////////////////////////////////////////
/// Set the footer rule size in points. By default, the footer rule is one half
/// point (1/144 inch). To turn off the rule, set the rule size to 0. The rule
/// will be  drawn just above the footer area.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setFooterRule(double pointsize)
{
    mFooterRule = qMax(0.0, pointsize);
}

///////////////////////////////////////////////////////////////////////////////
// footerText()
///////////////////////////////////////////////////////////////////////////////
/// Return the text for the footer.
///////////////////////////////////////////////////////////////////////////////

const QString &QPrintObject::footerText() const
{
    return mFooterText;
}

///////////////////////////////////////////////////////////////////////////////
// setFooterText()
///////////////////////////////////////////////////////////////////////////////
/// Set the text for the footer. Rich text is supported. HTML tags may be used
/// to format the text and align elements. The following page variables may be
/// included in the text:
/// - \&page; - Insert current page number
/// - \&date; - Insert current date, using the format set with setPageFormat()
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setFooterText(const QString &text)
{
    mFooterText = text;
}

///////////////////////////////////////////////////////////////////////////////
// dateFormat()
///////////////////////////////////////////////////////////////////////////////
/// Return the currently set date format string.
///////////////////////////////////////////////////////////////////////////////

const QString &QPrintObject::dateFormat() const
{
    return mDateFormat;
}

///////////////////////////////////////////////////////////////////////////////
// setDateFormat()
///////////////////////////////////////////////////////////////////////////////
/// Set the date format to be used for the \&date; page variable. The format
/// is the same as that used by QDate::toString(). If no format is set, the
/// format defaults to Qt::TextDate.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::setDateFormat(const QString &format)
{
    mDateFormat = format;
}

///////////////////////////////////////////////////////////////////////////////
// print()
///////////////////////////////////////////////////////////////////////////////
/// Print a document. A standard print dialog will be displayed. If a QPrinter
/// is not provided, a temporary printer object will be used. If the caption
/// parameter is not empty, it will be used as the title for the print
/// dialog.
///
/// \todo add overload taking html string
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::print(const QTextDocument *document,
                         const QString &caption)
{
    if (!document) return;

    // setup printer
    mPrinter->setOutputFormat(QPrinter::NativeFormat);
    mPrinter->setOutputFileName(QString());

    // show print dialog
    QPrintDialog dialog(mPrinter, mParent);
    dialog.setWindowTitle(caption.isEmpty() ? "Print Document" : caption);
    if (dialog.exec() == QDialog::Rejected) return;

    // print it
    mTempDoc = document->clone();
    print(mPrinter);

    delete mTempDoc;
    mTempDoc = 0;
}

///////////////////////////////////////////////////////////////////////////////
// preview()
///////////////////////////////////////////////////////////////////////////////
/// Displays a print preview dialog. If caption is not empty, it will be used
/// as the title of the dialog.
///////////////////////////////////////////////////////////////////////////////

void QPrintObject::preview(const QTextDocument *document,
                           const QString &caption)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
    if (!document) return;

    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(mPrinter, mParent);
    dialog->setWindowTitle(caption.isEmpty() ? "Print Preview" : caption);

    connect(dialog, SIGNAL(paintRequested(QPrinter*)),
            this, SLOT(print(QPrinter*)));

    // preview it
    mTempDoc = document->clone();
    dialog->exec();

    delete mTempDoc;
    mTempDoc = 0;

    delete dialog;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// private methods
///////////////////////////////////////////////////////////////////////////////

// paperRect() ////////////////////////////////////////////////////////////////
// Return the size of the paper, adjusted for DPI

QRectF QPrintObject::paperRect(QPaintDevice *device)
{
    // calculate size of paper
    QRectF rect = mPrinter->paperRect();
    // adjust for DPI
    rect.setWidth(rect.width() *
                  device->logicalDpiX() / mPrinter->logicalDpiX());
    rect.setHeight(rect.height() *
                   device->logicalDpiY() / mPrinter->logicalDpiY());

    return rect;
}

// contentRect() //////////////////////////////////////////////////////////////
// calculate the rect for the content block

QRectF QPrintObject::contentRect(QPaintDevice *device)
{
    // calculate size of content (paper - margins)
    QRectF rect = paperRect(device);

    rect.adjust(mmToInches(mLeftMargin) * device->logicalDpiX(),
                mmToInches(mTopMargin) * device->logicalDpiY(),
                -mmToInches(mRightMargin) * device->logicalDpiX(),
                -mmToInches(mBottomMargin) * device->logicalDpiY());

    // header
    if (mHeaderSize > 0) {
        rect.adjust(0, mmToInches(mHeaderSize) * device->logicalDpiY(), 0, 0);
        rect.adjust(0, mmToInches(mSpacing) * device->logicalDpiY(), 0, 0);
    }
    // footer
    if (mFooterSize > 0) {
        rect.adjust(0, 0, 0, -mmToInches(mFooterSize) * device->logicalDpiY());
        rect.adjust(0, 0, 0, -mmToInches(mSpacing) * device->logicalDpiY());
    }

    return rect;
}

// headerRect() //////////////////////////////////////////////////////////////
// calculate the rect for the header block

QRectF QPrintObject::headerRect(QPaintDevice *device)
{
    QRectF rect = paperRect(device);
    rect.adjust(mmToInches(mLeftMargin) * device->logicalDpiX(),
                mmToInches(mTopMargin) * device->logicalDpiY(),
                -mmToInches(mRightMargin) * device->logicalDpiX(), 0);

    (mHeaderRule / 144.0);

    rect.setBottom(rect.top() +
                   mmToInches(mHeaderSize) * device->logicalDpiY());

    return rect;
};

// footerRect() ///////////////////////////////////////////////////////////////
// calculate the rect for the footer block

QRectF QPrintObject::footerRect(QPaintDevice *device)
{
    QRectF rect = paperRect(device);
    rect.adjust(mmToInches(mLeftMargin) * device->logicalDpiX(), 0,
                -mmToInches(mRightMargin) * device->logicalDpiX(),
                -mmToInches(mBottomMargin) * device->logicalDpiY());

    rect.setTop(rect.bottom() -
                mmToInches(mFooterSize) * device->logicalDpiY());

    return rect;
};

// print() ////////////////////////////////////////////////////////////////////
// Common printing routine. Print mTempDoc to given printer device.

void QPrintObject::print(QPrinter *printer)
{
    if (!printer || !mTempDoc) return;

    mTempDoc->setUseDesignMetrics(true);
    mTempDoc->documentLayout()->setPaintDevice(printer);
    mTempDoc->setPageSize(contentRect(printer).size());
    // dump existing margin (if any)
    QTextFrameFormat fmt = mTempDoc->rootFrame()->frameFormat();
    fmt.setMargin(0);
    mTempDoc->rootFrame()->setFrameFormat(fmt);

    // to iterate through pages we have to worry about
    // copies, collation, page range, and print order

    // get num copies
    int doccopies;
    int pagecopies;
    if (printer->collateCopies()) {
        doccopies = 1;
        pagecopies = printer->numCopies();
    } else {
        doccopies = printer->numCopies();
        pagecopies = 1;
    }

    // get page range
    int firstpage = printer->fromPage();
    int lastpage = printer->toPage();
    if (firstpage == 0 && lastpage == 0) { // all pages
        firstpage = 1;
        lastpage = mTempDoc->pageCount();
    }

    // print order
    bool ascending = true;
    if (printer->pageOrder() == QPrinter::LastPageFirst) {
        int tmp = firstpage;
        firstpage = lastpage;
        lastpage = tmp;
        ascending = false;
    }

    // loop through and print pages
    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform, true);
    for (int dc=0; dc<doccopies; dc++) {
        int pagenum = firstpage;
        while (true) {
            for (int pc=0; pc<pagecopies; pc++) {
                if (printer->printerState() == QPrinter::Aborted ||
                        printer->printerState() == QPrinter::Error) {
                    return;
                }
                // print page
                paintPage(&painter, mTempDoc, pagenum);
                if (pc < pagecopies-1) printer->newPage();
            }
            if (pagenum == lastpage) break;
            if (ascending) pagenum++;
            else           pagenum--;
            printer->newPage();
        }

        if (dc < doccopies-1) printer->newPage();
    }
}

// paintPage() ////////////////////////////////////////////////////////////////
// paint an individual page of the document to the painter

void QPrintObject::paintPage(QPainter *painter,
                             QTextDocument *document,
                             int pagenum)
{
    QRectF rect;
    double onepoint = painter->device()->logicalDpiY() / 72.0;

    // header
    if (mHeaderSize > 0) {
        rect = headerRect(painter->device());
        if (mHeaderRule > 0.0) {
            painter->save();
            // allow space between rule and header
            painter->translate(0, onepoint + (mHeaderRule * onepoint / 2.0));
            painter->setPen(QPen(Qt::black, mHeaderRule * onepoint));
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
            painter->restore();
        }

        // replace page variables
        QString header = mHeaderText;
        header.replace("&page;", QString::number(pagenum));
        if (mDateFormat.isEmpty()) {
            header.replace("&date;", QDate::currentDate().toString());
        } else {
            header.replace("&date;", QDate::currentDate().toString(mDateFormat));
        }

        painter->save();
        painter->translate(rect.left(), rect.top());
        QRectF clip(0, 0, rect.width(), rect.height());
        QTextDocument doc;
        doc.setUseDesignMetrics(true);
        doc.setHtml(header);
        doc.documentLayout()->setPaintDevice(painter->device());
        doc.setPageSize(rect.size());

        // align text to bottom
        double newtop = clip.bottom() - doc.size().height();
        clip.setHeight(doc.size().height());
        painter->translate(0, newtop);

        doc.drawContents(painter, clip);
        painter->restore();
    }

    // footer
    if (mFooterSize > 0) {
        rect = footerRect(painter->device());
        if (mFooterRule > 0.0) {
            painter->save();
            // allow space between rule and footer
            painter->translate(0, -onepoint + (-mFooterRule * onepoint / 2.0));
            painter->setPen(QPen(Qt::black, mFooterRule * onepoint));
            painter->drawLine(rect.topLeft(), rect.topRight());
            painter->restore();
        }

        // replace page variables
        QString footer = mFooterText;
        footer.replace("&page;", QString::number(pagenum));
        if (mDateFormat.isEmpty()) {
            footer.replace("&date;", QDate::currentDate().toString());
        } else {
            footer.replace("&date;", QDate::currentDate().toString(mDateFormat));
        }

        painter->save();
        painter->translate(rect.left(), rect.top());
        QRectF clip(0, 0, rect.width(), rect.height());
        QTextDocument doc;
        doc.setUseDesignMetrics(true);
        doc.setHtml(footer);
        doc.documentLayout()->setPaintDevice(painter->device());
        doc.setPageSize(rect.size());
        doc.drawContents(painter, clip);
        painter->restore();
    }

    // content
    painter->save();

    rect = contentRect(painter->device());
    painter->translate(rect.left(), rect.top() - (pagenum-1) * rect.height());
    QRectF clip(0, (pagenum-1) * rect.height(), rect.width(), rect.height());

    document->drawContents(painter, clip);

    painter->restore();
}

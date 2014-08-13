#include "exportobject.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QVariant>

int ExportObject::export2Excel()
{
    if(m_FieldList.size() <= 0)
    {
        qDebug() << "ExportExcelObject::export2Excel failed: No fields defined.";
        return -1;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "excelexport");
    if(!db.isValid())
    {
        qDebug() << "ExportExcelObject::export2Excel failed: QODBC not supported.";
        return -2;
    }
    // set the dsn string
    QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%1\";DBQ=%2").
                  arg(m_ExcelFilePath).arg(m_ExcelFilePath);
    db.setDatabaseName(dsn);
    if(!db.open())
    {
        qDebug() << "ExportExcelObject::export2Excel failed: Create Excel file failed by DRIVER={Microsoft Excel Driver (*.xls)}.";
        //QSqlDatabase::removeDatabase("excelexport");
        return -3;
    }

    QSqlQuery query(db);

    //drop the table if it's already exists
    QString s, sSql = QString("DROP TABLE [%1] (").arg(m_SheetName);
    query.exec(sSql);

    //create the table (sheet in Excel file)
    sSql = QString("CREATE TABLE [%1] (").arg(m_SheetName);
    for (int i = 0; i < m_FieldList.size(); i++)
    {
        s = QString("[%1] %2").arg(m_FieldList.at(i)->sFieldName).arg(m_FieldList.at(i)->sFieldType);
        sSql += s;
        if(i < m_FieldList.size() - 1)
            sSql += " , ";
    }

    sSql += ")";
    query.prepare(sSql);

    if(!query.exec())
    {
        qDebug() << "ExportExcelObject::export2Excel failed: Create Excel sheet failed.";
        //db.close();
        //QSqlDatabase::removeDatabase("excelexport");
        return -4;
    }

    //add all rows
    sSql = QString("INSERT INTO [%1] (").arg(m_SheetName);
    for (int i = 0; i < m_FieldList.size(); i++)
    {
        sSql += m_FieldList.at(i)->sFieldName;
        if(i < m_FieldList.size() - 1)
            sSql += " , ";
    }
    sSql += ") VALUES (";
    for (int i = 0; i < m_FieldList.size(); i++)
    {
        sSql += QString(":data%1").arg(i);
        if(i < m_FieldList.size() - 1)
            sSql += " , ";
    }
    sSql += ")";

    qDebug() << sSql;

    int r, iRet = 0;
    for(r = 0 ; r < m_TableView->model()->rowCount() ; r++)
    {
        query.prepare(sSql);
        for (int c = 0; c < m_FieldList.size(); c++)
        {
            query.bindValue(QString(":data%1").arg(c), m_TableView->model()->data(m_TableView->model()->index(r, m_FieldList.at(c)->iCol)));
        }

        if(query.exec())
            iRet++;

        if(r % 10 == 0)
            emit exportedRowCount(r);
    }

    emit exportedRowCount(r);

    return iRet;
}
void ExportObject::ExportExcel ()
{
//    QString fileName = QFileDialog::getSaveFileName(this, \
//              QObject::tr("Save <a href=\"http://www.it165.net/edu/ebg/\" target=\"_blank\" \
//              class=\"keylink\">excel</a>"), \
//              ".", tr("Microsoft Office 2003 (*.xls)"));
//            //获取保存路径
//    if (fileName.isEmpty()) {
//        QMessageBox::critical(0, tr("错误"), tr("要保存的文件名为空！"));
//        return;
//    }
//    //ExportObject obj(fileName,"help", ui->treeView) ;
//    //obj.addField (0,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.addField (1,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.addField (2,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.addField (3,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.addField (4,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.addField (5,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.addField (6,QObject::tr("cul1"), "VCHAR(10)");
//    //obj.export2Excel ();
//    //return;
//    //建立Excel对象
//    QAxObject *excel = new QAxObject("Excel.Application");

//    excel->dynamicCall("SetVisible(bool)", true); //如果为了看自己的程序到底怎样工作，可以设置为true  www.it165.net
//    excel->setProperty("Visible", false);
//    QAxObject *workbooks = excel->querySubObject("WorkBooks");
//    workbooks->dynamicCall("Add");
//    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
//    QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
////    QAxObject *range = worksheet->querySubObject("Cells(int,int)", mModel->columnCount (),mModel->rowCount ());
//   // mModel->columnCount ();
//    //mModel->rowCount ();
//    int count = 0;
////    mModel->index (0,0);
////    mModel->index ();
//    QProgressDialog dlg;
//    QObject::connect (this, SIGNAL(signalCount(int)), &dlg, SLOT(setValue(int)));
//    dlg.show ();
//    QAxObject *cellX;
//    char XX[]={"ABCDEFGHIGKLMNOPQRSTUVWXYZ"};
//    qDebug() << "Column: " << mModel->columnCount () << ", Row:" << mModel->rowCount ();
////    for(count=1; count <mModel->rowCount ();count++){
//    for(count=1; count <mModel->rowCount ();count++){
//        emit signalCount ((count*100)/mModel->rowCount ());
//        for(int i=0;i<mModel->columnCount ();i++){
//           QString X=XX[i]+QString::number(count);//设置要操作的单元格，如A1
//           cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);//获取单元格
//           cellX->dynamicCall("SetValue(const QVariant&)",QVariant(mModel->record (count).value (i)));
//        }
//    }


//    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));
//    QMessageBox::information(this, tr("OK"), tr("保存成功！"));
//    workbook->dynamicCall("Close()");
//    worksheet->clear();//释放所有工作表
//    excel->dynamicCall("Quit()");
//    delete excel;//释放excel
}

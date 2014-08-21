#ifndef QPROTOCOLBASE_H
#define QPROTOCOLBASE_H
//typedef int size_t;
class QProtocolBase
{
public:
    QProtocolBase();//TypeIO &IODev):mIODev(IODev);
    virtual int direct_read();
    virtual int direct_write();
private:
    //    TypeIO mIODev;

};

#endif // QPROTOCOLBASE_H

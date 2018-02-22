#ifndef QDAQGPIB_H
#define QDAQGPIB_H

#include "QDaqInterface.h"
#include "QDaqGpibPlugin.h"
#include "QDaqTypes.h"

class QDaqGpib : public QDaqInterface
{
    Q_OBJECT

    QDaqGpibPlugin* gpib_;

public:
    Q_INVOKABLE explicit QDaqGpib(const QString& name);
    virtual ~QDaqGpib();

    // io
    virtual bool open_port(uint i, QDaqDevice*);
    virtual void close_port(uint i);
    virtual void clear_port(uint i);
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);

    // diagnostics
    virtual int readStatusByte(uint port);
    virtual bool isValidPort(uint i) { return i>0 && i<32; }

protected:
    // open-close
    virtual bool open_();
    virtual void clear_();
    virtual void setTimeout_(uint ms);

public slots:
    QDaqIntVector findListeners();
    QString listPlugins();
    bool loadPlugin(const QString& fname);

private:
    // error reporting
    void pushGpibError(int code, const QString& comm);

};

#endif // RTGPIB_H

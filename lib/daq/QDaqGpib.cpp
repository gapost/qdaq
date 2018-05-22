#include "QDaqGpib.h"
#include "qdaqpluginloader.h"


QDaqGpib::QDaqGpib(const QString& name) :
QDaqInterface(name), gpib_(0)
{
    // 32 empty places
    ports_.fill(0, 32);
}

QDaqGpib::~QDaqGpib()
{
}

void QDaqGpib::pushGpibError(int code, const QString& comm)
{
    if (gpib_)
        pushError(gpib_->errorMsg(code),comm);
    else
        pushError("Unknown Error",comm);
}

bool QDaqGpib::open_()
{
    if (!gpib_) return false;

    if (isOpen()) return true;

    os::auto_lock L(comm_lock);

    gpib_->SendIFC(address());

    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("SendIFC(%1)").arg(address())
            );
    }
    else QDaqInterface::open_();

    if (isOpen())
    {
        gpib_->setTimeout(address(), timeout());
    }

    emit propertiesChanged();

    return isOpen();
}

bool QDaqGpib::open_port(uint id, QDaqDevice* dev)
{
    if (!gpib_) return false;
    os::auto_lock L(comm_lock);
    if (QDaqInterface::open_port(id,dev))
    {
        gpib_->EnableRemote(address(), id);
        if (gpib_->hasError())
        {
            pushGpibError(
                gpib_->error(),
                QString("EnableRemote(%1,%2)").arg(address()).arg(id)
            );
            QDaqInterface::close_port(id);
            return false;
        }
        else return true;
    }
    else return false;
}

void QDaqGpib::close_port(uint id)
{
    if (!gpib_) return;
    os::auto_lock L(comm_lock);
    gpib_->EnableLocal(address(), id);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("EnableLocal(%1,%2)").arg(address()).arg(id)
        );
    }
    QDaqInterface::close_port(id);
}

void QDaqGpib::clear_port(uint id)
{
    if (!gpib_) return;
    os::auto_lock L(comm_lock);
    gpib_->DeviceClear(address(),id);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("DeviceClear(%1,%2)").arg(address()).arg(id)
            );
    }
}

int QDaqGpib::read(uint port, char* data, int len, int eoi)
{
    if (!gpib_) return 0;
    os::auto_lock L(comm_lock);
    gpib_->Receive(address(), port, data, len, eoi);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("Recieve(%1,%2,%3,%4)").arg(address()).arg(port).arg(len).arg(eoi)
            );
        return 0;
    }
    else return gpib_->count();
}

int QDaqGpib::readStatusByte(uint port)
{
    if (!gpib_) return 0;
    os::auto_lock L(comm_lock);
    int result = gpib_->ReadStatusByte (address(), port);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("ReadStatusByte(%1,%2)").arg(address()).arg(port)
            );
        return 0;
    }
    else return result;
}

int QDaqGpib::write(uint port, const char* buff, int len, int e)
{
    if (!gpib_) return 0;
    os::auto_lock L(comm_lock);
    int eot =  (e & 0x0000FF00) >> 8;
    gpib_->Send(address(), port, buff, len, eot);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("Send(%1,%2,%3,%4,%5)").arg(address()).arg(port)
                .arg(QString(QByteArray(buff,len))).arg(len).arg(eot)
            );
        return 0;
    }
    else return len; // ibcntl; linux-gpib sends +1 byte
}

void QDaqGpib::trigger(uint id)
{
    if (!gpib_) return;
    os::auto_lock L(comm_lock);
    gpib_->Trigger(address(),id);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("Trigger(%1,%2)").arg(address()).arg(id)
            );
    }
}

void QDaqGpib::clear_()
{
    if (!gpib_) return;
    os::auto_lock L(comm_lock);
    gpib_->SendIFC(address());
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("SendIFC(%1)").arg(address())
            );
    }
}

void QDaqGpib::setTimeout_(uint ms)
{
    if (!gpib_) return;
    os::auto_lock L(comm_lock);
    gpib_->setTimeout(address(),ms);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("setTimeout(%1,%2)").arg(address()).arg(ms)
            );
    }
}

QDaqIntVector QDaqGpib::findListeners()
{
    if (!gpib_) return QDaqIntVector();

    os::auto_lock L(comm_lock);

    QDaqIntVector Addresses(31), Listeners;

    for(int i=1; i<32; i++) Addresses[i-1] = i;

    gpib_->FindListeners(address(),Addresses,Listeners);
    if (gpib_->hasError())
    {
        pushGpibError(
            gpib_->error(),
            QString("FindListeners(%1,array,array)").arg(address())
            );
    }
    return Listeners;
}

QStringList QDaqGpib::listPlugins()
{
    return QDaqPluginLoader<QDaqGpibPlugin*>::findPlugins();
}

bool QDaqGpib::loadPlugin(const QString &fname)
{
    QObject *plugin = QDaqPluginLoader<QDaqGpibPlugin*>::loadPlugin(fname);

    if (plugin) {
        QDaqGpibPlugin* igpib = qobject_cast<QDaqGpibPlugin*>(plugin);
        if (igpib) {
            gpib_ = igpib;
            plugin->setParent(this);
        }
    }
    return gpib_!=0;
}


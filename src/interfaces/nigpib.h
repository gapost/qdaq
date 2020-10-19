#ifndef NIGPIB_H
#define NIGPIB_H

#include <QtCore/qglobal.h>

#if defined(NIGPIB_LIBRARY)
#  define NIGPIB_EXPORT Q_DECL_EXPORT
#else
#  define NIGPIB_EXPORT Q_DECL_IMPORT
#endif

#include "qdaqplugin.h"

#include "QDaqInterface.h"
#include "QDaqVector.h"
#include <QtPlugin>

class NIGPIB_EXPORT PluginFactory :
        public QObject,
        public QDaqPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqPluginIID FILE "qdaqnigpib.json")
    Q_INTERFACES(QDaqPlugin)

public:
    PluginFactory();

    virtual QList<const QMetaObject *> pluginClasses() const;
};


class NIGPIB_EXPORT QDaqNiGpib : public QDaqInterface
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit QDaqNiGpib(const QString& name);
    virtual ~QDaqNiGpib();

    // io
    virtual bool open_port(uint i, QDaqDevice*);
    virtual void close_port(uint i);
    virtual void clear_port(uint i);
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);
    /// Send the GPIB TRG signal to the device with address port
    virtual void trigger(uint port);

    // diagnostics
    /// Return the GPIB status byte for a device with address port.
    virtual int readStatusByte(uint port);
    virtual bool isValidPort(uint i) { return i>0 && i<32; }


protected:
    // open-close
    virtual bool open_();
    virtual void clear_();
    virtual void setTimeout_(uint ms);

public slots:
    /**
     * @brief Find connected devices on the GPIB interface.
     *
     * If the in interface supports it this function returns the
     * devices that are connected on the interface.
     *
     * @return A vector of GPIB addresses.
     */
    QDaqVector findListeners();

private:
    // error reporting
    void pushGpibError(int code, const QString& comm);

};

Q_DECLARE_METATYPE(QDaqNiGpib*)

#endif // NIGPIB_H

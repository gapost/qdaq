#ifndef LINUXGPIB_H
#define LINUXGPIB_H

#include "linux-gpib_global.h"

#include "qdaqplugin.h"

#include "QDaqInterface.h"
#include "QDaqTypes.h"
#include <QtPlugin>

class LINUXGPIBSHARED_EXPORT PluginFactory :
        public QObject,
        public QDaqPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqPluginIID FILE "qdaqlinuxgpib.json")
    Q_INTERFACES(QDaqPlugin)

public:
    PluginFactory();

    virtual QList<const QMetaObject *> pluginClasses() const;
};

/**
 * @brief A class for accesing a GPIB interface.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * Used for message-based communications with devices
 * connected on a IEEE-488 GPIB interface.
 *
 * The port numbers used in open_port() and other functions
 * correspond to GPIB addresses. A valid GPIB address is 1 to 32.
 * Address 0 is reserved for the interface controller wich is the
 * PC.
 *
 * Connection to specific GPIB hardware is performed via plugins.
 * A GPIB plugin implements the QDaqGpibPlugin interface.
 * See the Plugins module for the available plugins by default in QDaq.
 *
 * In order to use the class, first load a GPIB plugin and then call
 * open().
 *
 */

class LINUXGPIBSHARED_EXPORT QDaqLinuxGpib : public QDaqInterface
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit QDaqLinuxGpib(const QString& name);
    virtual ~QDaqLinuxGpib();

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
    QDaqIntVector findListeners();

private:
    // error reporting
    void pushGpibError(int code, const QString& comm);

};


#endif // LINUXGPIB_H

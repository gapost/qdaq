#ifndef QDAQGPIB_H
#define QDAQGPIB_H

#include "QDaqInterface.h"
#include "QDaqGpibPlugin.h"
#include "QDaqTypes.h"

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
    /**
     * @brief Return a list of available GPIB plugins.
     * For each plugin the file name is returned, which
     * can be passed to loadPlugin().
     * @return A string list of file names.
     */
    QStringList listPlugins();
    /**
     * @brief Loads the GPIB plugin specified by fname.
     * @param fname File name of required plugin.
     * @return True if the plugin is sucessfully loaded.
     */
    bool loadPlugin(const QString& fname);

private:
    // error reporting
    void pushGpibError(int code, const QString& comm);

};

#endif // RTGPIB_H

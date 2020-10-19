#ifndef _QDAQINTERFACE_H_
#define _QDAQINTERFACE_H_

#include "QDaqObject.h"

#include <QVector>

class QDaqDevice;

/**
 * @brief Base class of objects representing digital communications interfaces.
 *
 * @ingroup Daq
 *
 * QDaqInterface provides a common implementation for communication interfaces
 * as, e.g., serial (RS232,RS485,USB), ethernet (TCP/IP), MODBUS and GPIB.
 *
 * The QDaqInstrument class utilizes a descendant of QDaqInterface
 * for performing the actual communications with the instrument.
 *
 * A QDaqInterface has a number of communication "ports", which have different
 * meaning depending on the specific interface implementation.
 * On memory/register based interfaces like MODBUS, a port represents the
 * register number to be read/written. On interfaces that support connection
 * with multiple instruments (GPIB, RS485), the port is essentially the address of each
 * instrument.
 * On interfaces that support only
 * one-to-one communication with an instrument (RS232, TCP/IP) the port is not used.
 *
 */
class QDAQ_EXPORT QDaqInterface : public QDaqObject
{
	Q_OBJECT

    /** Address of this interface.
     *
     * The meaning of address depends on the type of interface.
     *
     * The address can be changed when the interface is closed.
     *
     */
	Q_PROPERTY(uint address READ address WRITE setAddress)
    /// Communications timeout in ms.
	Q_PROPERTY(uint timeout READ timeout WRITE setTimeout)
    /// Returns true if the interface is open.
	Q_PROPERTY(bool isOpen READ isOpen)

protected:
    // properties
	uint addr_, 
		timeout_; // ms
	bool isOpen_;

    // devices connected at the "ports" of this interface
    QVector<QDaqDevice*> ports_;

public:
    /// Construct a QDaqInterface with object-name name.
    explicit QDaqInterface(const QString& name);
    virtual ~QDaqInterface(void);

    /// When a QDaqInterface is detached it is also closed.
	virtual void detach();

	// getters
	uint address() const { return addr_; }
	uint timeout() const { return timeout_; }
	bool isOpen() const { return isOpen_; }

	// setters
	void setAddress(uint v);
    void setTimeout(uint v);

	// io
    /**
     * @brief Open communication port no. i for QDaqDevice d
     *
     * Returns true if the port was succesfully opened.
     */
    virtual bool open_port(uint i, QDaqDevice* d);
    /// Closes port i.
	virtual void close_port(uint i);
    /// Clear the communication channel with port i (interface specific).
	virtual void clear_port(uint i) { i=i; }
    /**
     * @brief Read data from a port.
     *
     * The meaning of the port parameter is different depending on
     * the type of communication interface.
     *
     * In message based communications (e.g. GPIB) the port is the
     * instrument address.
     *
     * In memory based interfaces (e.g. MODBUS) the port number
     * represents the starting register address to access. The number of
     * registers read/written depends on len, the size of the buffer in bytes.
     * A MODBUS register is 16 bit, ie, occupies 2 bytes.
     *
     * @param port Port number.
     * @param buff Pointer to memory buffer.
     * @param len Allocated number of bytes.
     * @param eos End of string byte
     * @return Number of bytes read.
     */
    virtual int read(uint port, char* buff, int len, int eos = 0);
    /**
     * @brief Write data to a port.
     *
     * The meaning of the port parameter is different depending on
     * the type of communication interface.
     *
     * In message based communications (e.g. GPIB) the port is the
     * instrument address.
     *
     * In memory based interfaces (e.g. MODBUS) the port number
     * represents the starting register address to access. The number of
     * registers read/written depends on len, the size of the buffer in bytes.
     * A MODBUS register is 16 bit, ie, occupies 2 bytes.
     *
     * @param p Port number.
     * @param buff Pointer to memory buffer.
     * @param len Number of bytes to write.
     * @param eos End of string byte
     * @return Number of bytes written.
     */
    virtual int write(uint port, const char* buff, int len, int eos = 0);
    /// Returns true is i is a valid port number.
	virtual bool isValidPort(uint i) { i=i; return true; }
    /// Returns a status byte for a port (interface specific).
	virtual int readStatusByte(uint port) { port=port; return 0; }
    /// Trigger a device connected to port (interface specific).
    virtual void trigger(uint port) { port=port; }

protected:
    /// Implementation of interface open.
	virtual bool open_() { return isOpen_ = true; }
    /// Implementation of interface close.
	virtual void close_();
    /// Implementation of interface clear.
	virtual void clear_() {}
    /// Implementation of setting the timeout.
    virtual void setTimeout_(uint v) { timeout_=v; }
    /// Throws a script error and a QDaqError if called on an open interface.
	bool throwIfOpen();

public slots:
    /// Opens the interface and returns true if succesful.
    bool open() { return open_(); }
    /// Closes the interface.
    void close() { close_(); }
    /// Clears the interface.
    void clear() { clear_(); }
};









#endif



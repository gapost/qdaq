#ifndef _QDAQDEVICE_H_
#define _QDAQDEVICE_H_

#include "QDaqTypes.h"
#include "QDaqJob.h"
#include "QDaqInterface.h"

#include <QByteArray>
#include <QVector>

class QDaqChannel;

/**
 * @brief A class representing a DAQ device.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * It is used in combination with a QDaqInterface in order
 * to communicate with the device.
 *
 * In order to start communications with a device, first instantiate
 * the interface and device objects, asign the interface to the device
 * and then open() the interface and bring the device online with on().
@code
QDaqRoot qdaq;
QDaqTcpip* tcp = new QDaqTcpip("tcp",'192.168.1.6',5000);
QDaqDevice* dev = new QDaqDevice("dev");
qdaq.appendChild(tcp);
qdaq.appendChild(dev);

dev->setInterface(tcp);
if (tcp->open() && dev->on())
  cout << "Device online!";
else
  cout << qdaq.errorBacktrace();
@endcode
 *
 * After the device is online the communication functions
 * can be used either for message based or register based
 * communication.
 *
 */
class QDaqDevice : public QDaqJob
{
	Q_OBJECT

    /// Size of the internal communication buffer.
	Q_PROPERTY(uint bufferSize READ bufferSize WRITE setBufferSize)
    /// Device address (depends on type of interface).
	Q_PROPERTY(int address READ address WRITE setAddress)
    /// True when device is online.
	Q_PROPERTY(bool online READ online WRITE setOnline)
    /// End of transmition code.
	Q_PROPERTY(int eot READ eot WRITE setEot)
    /// End of string code.
	Q_PROPERTY(int eos READ eos WRITE setEos)
	Q_PROPERTY(int inputChannels READ inputChannels)
	Q_PROPERTY(int outputChannels READ outputChannels)
    /// The underlying QDaqInterface used for communication.
    Q_PROPERTY(QDaqObject* interface READ interface WRITE setInterface)

protected:
    // properties
	int addr_;
	bool online_;
	int eot_;
	int eos_;

private:
    // comm buffer
    QByteArray buff_;
    uint buff_sz_;

protected:
    // my interface
    QPointer<QDaqInterface> ifc_;

    QVector<QDaqChannel*> inputChannels_;
    QVector<QDaqChannel*> outputChannels_;

	virtual bool arm_();

    /// Thow QDaqError and script error if called with device offline
	bool throwIfOffline();
    /// Thow QDaqError and script error if called with device online
	bool throwIfOnline();

public:
    Q_INVOKABLE
    /**
     * @brief QDaqDevice constructor.
     * @param name The QDaqObject name.
     */
    explicit QDaqDevice(const QString& name);
    virtual ~QDaqDevice();
	virtual void detach();

	// getters
    unsigned int bufferSize() const { return buff_sz_; }
	int address() const { return addr_; }
	bool online() const { return online_; }
	int eot() const { return eot_; }
	int eos() const { return eos_; }
	int inputChannels() const { return inputChannels_.size(); }
	int outputChannels() const { return outputChannels_.size(); }
    QDaqObject* interface() const { return ifc_; }

	// setters
	void setBufferSize(unsigned int sz);
	void setOnline(bool a);
	void setAddress(int a);
	void setEot(int e);
	void setEos(int a);
    void setInterface(QDaqObject *i);

	// channels
    QDaqChannel* getInputChannel(int i) { return inputChannels_[i]; }
    QDaqChannel* getOutputChannel(int i) { return outputChannels_[i]; }

	// io
    /// Write a C-string message to the device, returns the number of characters written.
    int write(const char* msg, int len);
    /// Write a zero terminated C-string message to the device, returns the number of characters written.
    int write(const char* msg);
    /// Write a byte array to the device, returns the number of bytes written.
    int write(const QByteArray& msg);
    /// Write a list byte arrays to the device, returns true if succesfull
    bool write(const QList<QByteArray>& msglist);

	virtual void checkError(const char* msg, int len) 
	{ Q_UNUSED(msg); Q_UNUSED(len); }

	//
	void forcedOffline(const QString& reason = QString());

protected:
    /**
     * @brief Implements specific procedures for bringing device on- or offline.
     *
     * This function is called to change the online property.
     *
     * Reimplement this function to define additional procedures needed in
     * devices classes that inherit QDaqDevice.
     *
     * In the base class implementation, QDaqInterface::open_port() is called
     * with the address passed as the port number. If this returns true, then
     * QDaqInterface::clear_port() is also called.
     *
     *
     * @param on True for making device online, false for offline.
     * @return True if succesful.
     */
	virtual bool setOnline_(bool on);

public slots:
    /// Try to bring the device online and return true if succesfull.
	bool on();
    /// Bring the device offline.
	void off();

    /**
     * @name Functions for message based communication.
     */
    /// Write a string message to the device. Return number of characters written.
    int write(const QString& msg);
    /// Write a byte array to the device. Return number of bytes written.
    int writeBytes(const QByteArray& msg);
    /// Read a string from the device. On error return empty string and emit QDaqError.
    QString read();
    /// Read bytes from the device. On error return empty array and emit QDaqError.
    QByteArray readBytes();
    /// Send message and return the answer, ie, read and write in one command.
    QString query(const QString& msg);

    /**
     * @name Functions for register based communication (eg, MODBUS).
     *
     * A register holds a 16-bit unsigned integer.
     */
    ///@{
    /**
     * @brief Write a value at a device register.
     * @param reg Register address.
     * @param val Value to write. Will be truncated to 16-bit (max 0xFFFF).
     * @return Non-zero on success.
     */
    int write(int reg, int val);
    /**
     * @brief Write to a number of consecutive device registers.
     * @param start_reg Starting register address.
     * @param n Number of registers.
     * @param msg Buffer containing data to write. Must hold at least 2*n bytes.
     * @return Bytes written.
     */
    int write(int start_reg, int n, const QByteArray& msg);
    /// Return the value of the device register at address reg.
    int read(int reg);
    /**
     * @brief Read a number of consecutive device registers.
     * @param reg Starting register address.
     * @param n Number of registers.
     * @return Register data in a byte array of length 2*n.
     */
    QByteArray read(int reg, int n);
    ///@}

    /// Call QDaqInterface::clear_port(). Depends on type of interface.
	void clear();

    /// Call QDaqInterface::readStatusByte(). Return value depends on type of interface.
	int statusByte();

};

#endif




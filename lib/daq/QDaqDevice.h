#ifndef _QDAQDEVICE_H_
#define _QDAQDEVICE_H_

#include "QDaqTypes.h"
#include "QDaqJob.h"
#include "QDaqInterface.h"

#include <QByteArray>
#include <QVector>

class QDaqChannel;

class QDaqDevice : public QDaqJob
{
	Q_OBJECT
	Q_PROPERTY(uint bufferSize READ bufferSize WRITE setBufferSize)
	Q_PROPERTY(int address READ address WRITE setAddress)
	Q_PROPERTY(bool online READ online WRITE setOnline)
	Q_PROPERTY(int eot READ eot WRITE setEot)
	Q_PROPERTY(int eos READ eos WRITE setEos)
	Q_PROPERTY(int inputChannels READ inputChannels)
	Q_PROPERTY(int outputChannels READ outputChannels)
    Q_PROPERTY(QDaqObject* interface READ interface WRITE setInterface)

protected:
	int addr_;
	bool online_;
	int eot_;
	int eos_;

private:
    QByteArray buff_;
    uint buff_sz_;

protected:
    QPointer<QDaqInterface> ifc_;

    QVector<QDaqChannel*> inputChannels_;
    QVector<QDaqChannel*> outputChannels_;

	virtual bool arm_();

	bool throwIfOffline();
	bool throwIfOnline();

public:
    Q_INVOKABLE explicit QDaqDevice(const QString& name);
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
	int write_(const char* msg, int len);
	int write_(const char* msg);
	int write_(const QByteArray& msg);
	bool write_(const QList<QByteArray>& msglist);
	QByteArray read_();
	int statusByte_();
	virtual void checkError(const char* msg, int len) 
	{ Q_UNUSED(msg); Q_UNUSED(len); }

	//
	void forcedOffline(const QString& reason = QString());

protected:
	// on-off implementation
	virtual bool setOnline_(bool on);

	// buffer alloc
	void alloc_(int sz);

	// io
    //int read__();

public slots:
	bool on();
	void off();

	int write(const QString& msg);
    int write(const QByteArray& msg);
    int write(int reg, int val);
    int write(int start_reg, const QByteArray& msg);

    QByteArray read();
    int read(int reg);
    QByteArray read(int reg, int n);

    QString query(const QString& msg);
	void clear();
	int statusByte();

};

#endif




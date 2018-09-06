#ifndef QDAQTCPIP_H
#define QDAQTCPIP_H

#include "interfaces_global.h"

#include "QDaqInterface.h"

#include <QHostAddress>

class tcp_socket;


/**
 * @brief A class for accesing a TCP/IP interface.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * QDaqTcpip performs communications with another host
 * using a tcp socket connection
 * to the specified host address and port. The port here
 * means the tcp/ip port and is not to be confused with
 * the ports of QDaqInterface.
 *
 * QDaqTcpip can be used in order to communicate with a single
 * device. For multiple devices connected on the ethernet bus, each with its
 * own host ip address,
 * a separate QDaqTcpip must be used per device.
 *
 * If the same device with single ip address offers several ports
 * for communication, then a separate QDaqTcpip must be used per
 * device port.
 *
 */
class INTERFACESSHARED_EXPORT QDaqTcpip : public QDaqInterface
{
    Q_OBJECT

    /// Tcp/ip port.
    Q_PROPERTY(uint port READ port WRITE setPort)
    /// Host IP address as string, eg '192.168.1.6'
    Q_PROPERTY(QString host READ host WRITE setHost)

protected:
    // properties
    uint port_;
    QHostAddress host_;
    // my socket
    tcp_socket* socket_;
public:

    /**
     * @brief QDaqTcpip constructor.
     *
     * It initializes the object with the interface is in the closed state.
     *
     * @param name QDaqObject name
     * @param ahost Host address
     * @param portno Port number where the host listens
     */
    Q_INVOKABLE explicit QDaqTcpip(const QString& name, const QString& ahost = QString(), uint portno = 0);
    virtual ~QDaqTcpip();

    // getters
    uint port() const { return port_; }
    QString host() const { return host_.toString(); }

    // setters
    void setPort(uint p);
    void setHost(const QString& h);


    virtual bool isValidPort(uint i) { return i==0; }

    // io
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);

protected:
    virtual bool open_();
    virtual void close_();
};

#endif // QDAQTCPIP_H

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <string>

class tcp_socket
{
    /* TCP port */
    int port;
    /* IP address */
    std::string ip;

    /* Slave address */
    int slave;
    /* Socket or file descriptor */
    int sfd;
    int debug;
    int error_recovery;
    int timo_s, timo_us; // timeout s, us

public:
    tcp_socket();
    ~tcp_socket();

    int connect(const char* aip, int aport);
    void disconnect();

    bool is_connected() const { return sfd > 0; }

    // return timo in ms
    int timeout() const
    {
        return timo_s*1000 + timo_us/1000;
    }
    void set_timeout(int ms);

    int send(const char* msg, int len);
    int receive(char* msg, int len);


    static const char* strerror(int errnum);

    static const char* lastErrorStr();
};

#endif // TCP_SOCKET_H

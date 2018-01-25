#include "tcp_socket.h"


#if defined(_WIN32)
# define OS_WIN32
/* ws2_32.dll has getaddrinfo and freeaddrinfo on Windows XP and later.
 * minwg32 headers check WINVER before allowing the use of these */
# ifndef WINVER
# define WINVER 0x0501
# endif
/* Already set in modbus-tcp.h but it seems order matters in VS2005 */
# include <winsock2.h>
# include <ws2tcpip.h>
# define SHUT_RDWR 2
# define close closesocket
#else

# include <unistd.h> // for close()

# include <sys/socket.h>
# include <sys/ioctl.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ < 5)
# define OS_BSD
# include <netinet/in_systm.h>
#endif

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL 0
#endif

#include <errno.h>

#define TCP_ENOBASE 112345678

#define EBADDATA (TCP_ENOBASE + 1)

/* Timeouts in microsecond (0.5 s) */
#define _RESPONSE_TIMEOUT    500000
#define _BYTE_TIMEOUT        500000


static int tcp_init(void)
{
#ifdef OS_WIN32
    /* Initialise Windows Socket API */
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        //errno = EIO;
        return -1;
    }
#endif
    return 0;
}

static int set_ipv4_options(int s)
{
    int rc;
    int option;

    /* Set the TCP no delay flag */
    /* SOL_TCP = IPPROTO_TCP */
    option = 1;
    rc = setsockopt(s, IPPROTO_TCP, TCP_NODELAY,
                    (const char *)&option, sizeof(int));
    if (rc == -1) {
        return -1;
    }

    /* If the OS does not offer SOCK_NONBLOCK, fall back to setting FIONBIO to
     * make sockets non-blocking */
    /* Do not care about the return value, this is optional */
    option = 1;
#if !defined(SOCK_NONBLOCK) && defined(FIONBIO)
#ifdef OS_WIN32
    {
        /* Setting FIONBIO expects an unsigned long according to MSDN */
        u_long loption = 1;
        ioctlsocket(s, FIONBIO, &loption);
    }
#else
    ioctl(s, FIONBIO, &option);
#endif
#endif

#ifndef OS_WIN32
    /**
     * Cygwin defines IPTOS_LOWDELAY but can't handle that flag so it's
     * necessary to workaround that problem.
     **/
    /* Set the IP low delay option */
    option = IPTOS_LOWDELAY;
    rc = setsockopt(s, IPPROTO_IP, IP_TOS,
                    (const void *)&option, sizeof(int));
    if (rc == -1) {
        return -1;
    }
#endif

    return 0;
}

static int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen,
                    const struct timeval *ro_tv)
{
    int rc = ::connect(sockfd, addr, addrlen);

#ifdef OS_WIN32
    int wsaError = 0;
    if (rc == -1) {
        wsaError = WSAGetLastError();
    }

    if (wsaError == WSAEWOULDBLOCK || wsaError == WSAEINPROGRESS) {
#else
    if (rc == -1 && errno == EINPROGRESS) {
#endif
        fd_set wset;
        int optval;
        socklen_t optlen = sizeof(optval);
        struct timeval tv = *ro_tv;

        /* Wait to be available in writing */
        FD_ZERO(&wset);
        FD_SET(sockfd, &wset);
        rc = ::select(sockfd + 1, NULL, &wset, NULL, &tv);
        if (rc <= 0) {
            /* Timeout or fail */
            return -1;
        }

        /* The connection is established if SO_ERROR and optval are set to 0 */
        rc = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&optval, &optlen);
        if (rc == 0 && optval == 0) {
            return 0;
        } else {
#ifdef OS_WIN32
            WSASetLastError(WSAECONNREFUSED);
#else
            errno = ECONNREFUSED;
#endif
            return -1;
        }
    }
    return rc;
}

static int _select(int fd, fd_set *rset, timeval *tv)
{
    int s_rc;
    while ((s_rc = ::select(fd+1, rset, NULL, NULL, tv)) == -1) {
#ifdef OS_WIN32
        if (WSAGetLastError()==WSAEINTR) {
#else
        if (errno == EINTR) {
#endif

            //if (debug) {
            //    fprintf(stderr, "A non blocked signal was caught\n");
            //}
            /* Necessary after an error */
            FD_ZERO(rset);
            FD_SET(fd, rset);
        } else {
            return -1;
        }
    }

    if (s_rc == 0) {
#ifdef OS_WIN32
        WSASetLastError(WSAETIMEDOUT);
#else
        errno = ETIMEDOUT;
#endif
        return -1;
    }

    return s_rc;
}


tcp_socket::tcp_socket() : port(0), ip(), slave(-1),
    sfd(-1), debug(0), error_recovery(0),
    timo_s(0), timo_us(300000) // 300 ms initial
{
}

tcp_socket::~tcp_socket()
{
    disconnect();
}

void tcp_socket::set_timeout(int ms)
{
    int s = ms/1000;
    timo_s = s;
    timo_us = (ms-s*1000)*1000;
}

/* Establishes a TCP connection with a server. */
int tcp_socket::connect(const char *aip, int aport)
{
    if (aip==NULL) {
#ifdef OS_WIN32
        WSASetLastError(WSAEINVAL);
#else
        errno = EINVAL;
#endif
        return -1;
    }
    ip.assign(aip);
    if (ip.size()>15) {
#ifdef OS_WIN32
        WSASetLastError(WSAEINVAL);
#else
        errno = EINVAL;
#endif
        return -1;
    }
    port = aport;

    int rc;
    /* Specialized version of sockaddr for Internet socket address (same size) */
    struct sockaddr_in addr;
    int flags = SOCK_STREAM;
    timeval response_timeout;

#ifdef OS_WIN32
    if (tcp_init() != 0) {
        return -1;
    }
#endif

#ifdef SOCK_CLOEXEC
    flags |= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
    flags |= SOCK_NONBLOCK;
#endif

    sfd = socket(PF_INET, flags, 0);
    if (sfd == -1) {
        return -1;
    }

    rc = set_ipv4_options(sfd);
    if (rc == -1) {
        close(sfd);
        sfd = -1;
        return -1;
    }

    //if (debug) {
    //    printf("Connecting to %s:%d\n", ip, port);
    //}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    response_timeout.tv_sec = timo_s;
    response_timeout.tv_usec = timo_us;
    rc = _connect(sfd, (struct sockaddr *)&addr, sizeof(addr), &response_timeout);
    if (rc == -1) {
#ifdef OS_WIN32
        int err_ = WSAGetLastError();
        close(sfd);
        sfd = -1;
        WSASetLastError(err_);
#else
        int err_ = errno; // close will clear it
        close(sfd);
        sfd = -1;
        errno = err_;
#endif
        return -1;
    }

    return 0;
}

void tcp_socket::disconnect()
{
    /* Closes the network connection and socket in TCP mode */
    if (sfd != -1) {
        shutdown(sfd, SHUT_RDWR);
        close(sfd);
        sfd = -1;
    }
}

int tcp_socket::send(const char *msg, int len)
{
    /* MSG_NOSIGNAL
       Requests not to send SIGPIPE on errors on stream oriented
       sockets when the other end breaks the connection.  The EPIPE
       error is still returned. */
    int rc = ::send(sfd,msg,len,MSG_NOSIGNAL);
    if (rc==-1) return -1;


    if (rc > 0 && rc != len) {
#ifdef OS_WIN32
        WSASetLastError(EBADDATA);
#else
        errno = EBADDATA;
#endif
        return -1;
    }

    return rc;
}

/* Read data into msg until either (a) len bytes have been written,
 * (b) the timeout has been reached
 */
int tcp_socket::receive(char *msg, int len)
{
    int rc;
    fd_set rset;

    /* Add a file descriptor to the set */
    FD_ZERO(&rset);
    FD_SET(sfd, &rset);

    //tv.tv_sec = response_timeout.tv_sec;
    //tv.tv_usec = response_timeout.tv_usec;
    //p_tv = &tv;
    timeval response_timeout;
    response_timeout.tv_sec = timo_s;
    response_timeout.tv_usec = timo_us;
    rc = _select(sfd, &rset, &response_timeout);
    if (rc == -1)
        return -1;


    rc = ::recv(sfd, msg, len, 0);
    if (rc == 0) {
#ifdef OS_WIN32
        WSASetLastError(WSAECONNRESET);
#else
        errno = ECONNRESET;
#endif
        rc = -1;
    }

    return rc;
}



const char *tcp_socket::strerror(int errnum) {
    if (errnum==0)
    {
        return 0;
    }
    switch (errnum) {
/*    case EMBXILFUN:
        return "Illegal function";
    case EMBXILADD:
        return "Illegal data address";
    case EMBXILVAL:
        return "Illegal data value";
    case EMBXSFAIL:
        return "Slave device or server failure";
    case EMBXACK:
        return "Acknowledge";
    case EMBXSBUSY:
        return "Slave device or server is busy";
    case EMBXNACK:
        return "Negative acknowledge";
    case EMBXMEMPAR:
        return "Memory parity error";
    case EMBXGPATH:
        return "Gateway path unavailable";
    case EMBXGTAR:
        return "Target device failed to respond";
    case EMBBADCRC:
        return "Invalid CRC";*/
    case EBADDATA:
        return "Invalid data";
/*    case EMBBADEXC:
        return "Invalid exception code";
    case EMBMDATA:
        return "Too many data";
    case EMBBADSLAVE:
        return "Response not from requested slave";*/
    default:
        return strerror(errnum);
    }
}

const char *tcp_socket::lastErrorStr()
{
#ifdef OS_WIN32
    // Retrieve the system error message for the last-error code

     static char MsgBuf[1024];
     DWORD dw = WSAGetLastError();

     int ret = FormatMessageA(
                 FORMAT_MESSAGE_FROM_SYSTEM |
                 FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,
                 dw,
                 1033,
                 (LPSTR) &MsgBuf,
                 1024, NULL );

     MsgBuf[ret] = 0;

     return MsgBuf;
#else
    return tcp_socket::strerror(errno);
#endif
}



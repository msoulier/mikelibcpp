#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <map>

#define MAX_BUFFER 1024

typedef unsigned long int SESSIONID;

enum NetworkManagerMode {
    UNSET=1,
    CLIENT,
    SERVER
};

/*
 * A manager for multiple sockets, either as a client or server
 * depending on how it was invoked.
 */
class NetworkManager
{
public:
    // Construtor
    NetworkManager();
    // Copy constructor
    NetworkManager(NetworkManager &source);
    // Destructor
    ~NetworkManager(void);
    // Assignment operator
    void operator=(NetworkManager &source);
    // A print method for sending data an iostream
    void Print(std::ostream *os);
    // Write data to an open socket
    ssize_t write(const std::string msg, SESSIONID sessionid=1);
    // Read data from an open socket
    ssize_t read(std::string &buffer, SESSIONID sessionid=1);

private:
    // Reusable Copy method
    void Copy(NetworkManager &source);

protected:
    // Note: socket fds for sessions are mapped in m_sessionmap.
    // The socket fd
    int m_sockfd;
    // The bind port.
    int m_bind_port;
    // Client or server?
    NetworkManagerMode m_mode;
    // Map of sessionids to network fds.
    std::map<SESSIONID,int> m_sessionmap;
    // An incrementing sessionid.
    SESSIONID m_latest_sessionid;

    // Setting the mode.
    void set_mode(NetworkManagerMode mode);
};

std::ostream &operator<<(std::ostream &os, NetworkManager &manager);

class TcpNetworkManager: public NetworkManager
{
public:
    TcpNetworkManager();
    ~TcpNetworkManager();
    // Connect to a remote host as a client.
    SESSIONID connect_to(std::string host, std::string port);
    int listen(int port);
    SESSIONID accept();
private:

};

std::ostream &operator<<(std::ostream &os, TcpNetworkManager &manager);

class UdpNetworkManager: public NetworkManager
{
public:
    UdpNetworkManager();
    ~UdpNetworkManager();

private:

};

std::ostream &operator<<(std::ostream &os, UdpNetworkManager &manager);

#endif

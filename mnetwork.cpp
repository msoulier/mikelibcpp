#include <iostream>
#include <string.h>
#include <sstream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "mnetwork.hpp"
#include "mnet.h"

/*
 * NetworkManager
 */

NetworkManager::NetworkManager() :
    m_bind_port(0),
    m_mode(NetworkManagerMode::UNSET),
    m_latest_sessionid(0)
{}

NetworkManager::~NetworkManager()
{
    // Unconditionally shut down the socket and close it.
    shutdown(m_sockfd, SHUT_RDWR);
    close(m_sockfd);
}

void NetworkManager::set_mode(NetworkManagerMode mode) {
    // Can only set mode from UNSET.
    if (m_mode == NetworkManagerMode::UNSET) {
        m_mode = mode;
    } else {
        throw std::runtime_error("mode already set");
    }
}

ssize_t NetworkManager::write(const std::string msg, SESSIONID sessionid) {
    assert( m_mode != NetworkManagerMode::UNSET );
    int fd;
    if (m_sessionmap.count(sessionid) == 1) {
        fd = m_sessionmap[sessionid];
    } else {
        std::cerr << "write called with unknown sessionid " << sessionid << std::endl;
        return -1;
    }

    if (fd == 0) {
        std::cerr << "write called on a closed socket" << std::endl;
        return -1;
    }
    assert( msg.size() <= MAX_BUFFER );
    int bytes = ::write(fd, msg.c_str(), msg.size());
    return bytes;
}

ssize_t NetworkManager::read(std::string &buffer, SESSIONID sessionid) {
    assert( m_mode != NetworkManagerMode::UNSET );
    int fd;
    if (m_sessionmap.count(sessionid) == 1) {
        fd = m_sessionmap[sessionid];
    } else {
        std::cerr << "read called with unknown sessionid " << sessionid << std::endl;
        return -1;
    }
    char cbuffer[MAX_BUFFER];
    size_t size = MAX_BUFFER;
    // Note, read will not NULL-terminate the string, but the std::string.assign() will.
    int bytes_read = ::read(fd, cbuffer, size);
    if (bytes_read < 0) {
        perror("read");
        return bytes_read;
    }
    buffer.assign(cbuffer, bytes_read);
    return bytes_read;
}

/*
 * TcpNetworkManager
 */
TcpNetworkManager::TcpNetworkManager()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
}

TcpNetworkManager::~TcpNetworkManager()
{}

SESSIONID TcpNetworkManager::connect_to(std::string host, std::string port)
{
    set_mode(NetworkManagerMode::CLIENT);
    SESSIONID rv;

    m_sockfd = connect_tcp_client((const char*)host.c_str(), (const char*)port.c_str());
    if (m_sockfd < 0) {
        rv = m_sockfd;
    } else {
        // FIXME: set the m_bind_port after connecting
        m_sessionmap[++m_latest_sessionid] = m_sockfd;
        rv = m_latest_sessionid;
    }
    return rv;
}

int TcpNetworkManager::listen(int port) {
    struct sockaddr_in servaddr;
    set_mode(NetworkManagerMode::SERVER);

    // socket create and verification
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) {
        perror("socket creation failed...\n");
        return 0;
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    // FIXME: need a bind parameter
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(m_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        perror("socket bind failed...\n");
        return 0;
    }

    // Now server is ready to listen and verification
    if ((::listen(m_sockfd, 5)) != 0) {
        perror("Listen failed...\n");
        return 0;
    }
    return 1;
}

SESSIONID TcpNetworkManager::accept() {
    socklen_t len;
    struct sockaddr_in client;
    int serverfd = 0;
    char client_address[INET_ADDRSTRLEN];

    len = sizeof(client);

    // Accept the data packet from client and verification
    serverfd = ::accept(m_sockfd, (struct sockaddr*)&client, &len);
    if (serverfd < 0) {
        perror("server accept failed...\n");
        return 0;
    }
    m_sessionmap[++m_latest_sessionid] = serverfd;
    if (inet_ntop(AF_INET, &(client.sin_addr), client_address, INET_ADDRSTRLEN) == NULL) {
        std::cerr << "failed to resolve client address: " << strerror(errno) << std::endl;
        strcpy(client_address, "Unknown");
    }
    return m_latest_sessionid;
}

std::string TcpNetworkManager::print() {
    std::stringstream stream;
    stream << "TcpNetworkManager: " << "[" << "FIXME" << "]";
    return stream.str();
}

std::ostream &operator<<(std::ostream &os, TcpNetworkManager &manager) {
    return os << manager.print();
}

/*
 * UdpNetworkManager
 */
UdpNetworkManager::UdpNetworkManager()
{
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

UdpNetworkManager::~UdpNetworkManager()
{}

std::string UdpNetworkManager::print() {
    std::stringstream stream;
    stream << "UdpNetworkManager: " << "[" << "FIXME" << "]";
    return stream.str();
}

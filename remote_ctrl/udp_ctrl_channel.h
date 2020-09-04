#pragma once

#if defined(_WIN32)
#include <winsock2.h>
#endif
#include <thread>
#include "ctrl_channel.h"

#if defined(_WIN32)

struct UdpConnectInfo {
    std::string local_ip;
    uint16_t local_port;
    std::string remote_ip;
    uint16_t remote_port;
    bool send;
    bool recv;
};

class REMOTE_CTRL_API UdpClient : public IIOClient {
public:
    UdpClient();
    virtual ~UdpClient();
    virtual void setCallbacks(IOCallbacks &&cbs) override;
    virtual int connect(void *info) override;
    virtual int sendmsg(const char *buf, int len) override;
    virtual void close() override;
private:
    SOCKET m_fd;
    SOCKADDR_IN m_localAddr;
    SOCKADDR_IN m_remoteAddr;
    std::unique_ptr<std::thread> m_recvThread;
    IOCallbacks m_cbs;
    bool m_run;
};

#endif


#include "udp_ctrl_channel.h"

#if defined(_WIN32)
UdpClient::UdpClient() : m_fd(INVALID_SOCKET) {
}

UdpClient::~UdpClient() {
}

void UdpClient::setCallbacks(IOCallbacks &&cbs) {
    m_cbs = cbs;
}

int UdpClient::connect(void *info) {
    m_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_fd == INVALID_SOCKET) {
        return -1;
    }
    UdpConnectInfo *udpInfo = (UdpConnectInfo *)info;
    if (udpInfo->send) {
        m_remoteAddr.sin_family = AF_INET;
        m_remoteAddr.sin_port = htons(udpInfo->remote_port);
        m_remoteAddr.sin_addr.s_addr = inet_addr(udpInfo->remote_ip.c_str());
    }

    if (udpInfo->recv) {
        m_localAddr.sin_family = AF_INET;
        m_localAddr.sin_port = htons(udpInfo->local_port);
        m_localAddr.sin_addr.s_addr = htonl(INADDR_ANY);


        if (bind(m_fd, (SOCKADDR *)&m_localAddr, sizeof(m_localAddr)) == SOCKET_ERROR)
        {
            closesocket(m_fd);
            m_fd = INVALID_SOCKET;
            return -2;
        }
        int addrLen = sizeof(m_localAddr);
        getsockname(m_fd, (SOCKADDR *)&m_localAddr, (int *)&addrLen);

        if (m_cbs.on_connect != nullptr)
            m_cbs.on_connect(this, true);

        m_run = true;
        m_recvThread.reset(new std::thread([this]() {
            char buf[1024] = { 0 };
            int buflen = 1024;
            int recvLen = 0;
            SOCKADDR_IN remoteAddr;
            int remoteAddrSize = sizeof(remoteAddr);

            struct timeval timeout;
            struct fd_set fds;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            while (m_run)
            {
                // Setup fd_set structure
                FD_ZERO(&fds);
                FD_SET(m_fd, &fds);

                int ret = select(m_fd + 1, &fds, 0, 0, &timeout);
                if (ret < 0) {
                    if (m_run && m_cbs.on_error != nullptr)
                        m_cbs.on_error(this, WSAGetLastError());
                    break;
                }
                else if (ret == 0) {
                    // time out
                    continue;
                }

                recvLen = recvfrom(m_fd, buf, buflen,
                    0, (SOCKADDR *)&remoteAddr, &remoteAddrSize);
                if (recvLen > 0)
                {
                    if (m_cbs.on_data != nullptr)
                        m_cbs.on_data(this, buf, recvLen, NULL);
                }
                else {
                    if (m_run && m_cbs.on_error != nullptr)
                        m_cbs.on_error(this, WSAGetLastError());
                    break;
                }
                // getpeername(ReceivingSocket, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
            }
        }));
    }
    return 0;
}

int UdpClient::sendmsg(const char *buf, int len) {
    return sendto(m_fd, buf, len, 0,
        (SOCKADDR *)&m_remoteAddr, sizeof(m_remoteAddr));
}

void UdpClient::close() {
    m_run = false;
    closesocket(m_fd);
    m_fd = INVALID_SOCKET;

    if (m_recvThread != nullptr) {
        m_recvThread->join();
        m_recvThread.reset();
    }
}
#endif

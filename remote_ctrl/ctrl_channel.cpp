#include "ctrl_channel.h"
#include "udp_ctrl_channel.h"

IIOClient *createIOClient(IO_CLIENT_TYPE type, void *info)
{
    IIOClient *client = NULL;
    switch (type) {
#if defined(_WIN32)
    case UDP_CLIENT:
        client = new UdpClient();
        break;
#endif
    case RTM_CLIENT:
        break;
    default:
        break;
    }
    return client;
}


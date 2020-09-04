#pragma once

#ifdef REMOTE_CTRL_EXPORTS
#define REMOTE_CTRL_API __declspec(dllexport)
#else
#define REMOTE_CTRL_API __declspec(dllimport)
#endif

#include <functional>

class IIOClient;

enum IO_CLIENT_TYPE {
    UDP_CLIENT = 0,
    RTM_CLIENT
};

struct IOCallbacks {
    typedef std::function<void(IIOClient* client, bool connected)> connect_cb_type;
    typedef std::function<int(IIOClient* client, const char *data, size_t length, void *extInfo)> data_cb_type;
    typedef std::function<void(IIOClient* client, int err)> event_cb_type;
    connect_cb_type on_connect;
    data_cb_type on_data;
    event_cb_type on_error;
    IOCallbacks(connect_cb_type&& connect_cb = nullptr,
        data_cb_type&& data_cb = nullptr,
        event_cb_type&& error_cb = nullptr)
        : on_connect(std::move(connect_cb))
        , on_data(std::move(data_cb))
        , on_error(std::move(error_cb))
    {
    }
};

class IIOClient {
public:
    virtual ~IIOClient() {}
    virtual void setCallbacks(IOCallbacks &&cbs) = 0;
    virtual int connect(void *info) = 0;
    virtual int sendmsg(const char *buf, int len) = 0;
    virtual void close() = 0;
};

REMOTE_CTRL_API IIOClient *createIOClient(IO_CLIENT_TYPE type, void *info);


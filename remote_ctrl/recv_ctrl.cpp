#include "ctrl_channel.h"
#include "ctrl_msg.h"
#include "remote_ctrl.h"
#include <vector>

IIOClient * g_ctrlServer = NULL;

static FILE *g_log = NULL;

inline static void log(const char *fmt, ...) {
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    if (g_log != NULL)
        fwrite(buf, 1, strlen(buf), g_log);
}

static void onConnect(IIOClient* client, bool connected) {
}

static int onData(IIOClient* client, const char *data, size_t length, void *extInfo) {
    int ret = 0;
    std::vector<INPUT> inputs;
    while (length > 0) {
        EventMsgBase *msgBase = (EventMsgBase *)data;
        if (length < sizeof(EventMsgBase) || length < msgBase->length) {
            log("msg format error, data length not enough %d\n", length);
            ret = -1;
            break;
        }
        //HWND fw = GetForegroundWindow();
        // if (fw == NULL) {
            //fw = GetDesktopWindow();
        // }
        switch (msgBase->type) {
        case MOUSE_EVENT_TYPE: {
                MouseEventMsg *msg = (MouseEventMsg *)data;
                log("[MOUSE] event: %s, x: %d, y: %d, exData: 0x%x\n",
                    getMouseEventName(msg->mouse_event), msg->x, msg->y, msg->extData);
                INPUT in;
                in.type = INPUT_MOUSE;
                in.mi.dx = msg->x;
                in.mi.dy = msg->y;
                in.mi.mouseData = 0;
                in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
                in.mi.time = 0;
                in.mi.dwExtraInfo = 0;
                switch (msg->mouse_event) {
                case WM_MOUSEMOVE:
                    in.mi.dwFlags |= MOUSEEVENTF_MOVE;
                    break;
                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
                    in.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
                    break;
                case WM_LBUTTONUP:
                    in.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
                    break;
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                    in.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
                    break;
                case WM_RBUTTONUP:
                    in.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
                    break;
                case WM_MBUTTONDBLCLK:
                case WM_MBUTTONDOWN:
                    in.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
                    break;
                case WM_MBUTTONUP:
                    in.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
                    break;
                case WM_MOUSEWHEEL:
                    in.mi.mouseData = msg->extData >> 16;
                    in.mi.dwFlags |= MOUSEEVENTF_WHEEL;
                    break;
                case WM_MOUSEHWHEEL:
                    in.mi.mouseData = msg->extData >> 16;
                    in.mi.dwFlags |= MOUSEEVENTF_HWHEEL;
                    break;
                case WM_XBUTTONDBLCLK:
                case WM_XBUTTONDOWN:
                    in.mi.mouseData = msg->extData & 0x0F;
                    in.mi.dwFlags |= MOUSEEVENTF_XDOWN;
                    break;
                case WM_XBUTTONUP:
                    in.mi.mouseData = msg->extData & 0x0F;
                    in.mi.dwFlags |= MOUSEEVENTF_XUP;
                    break;
                default:
                    in.mi.dwFlags |= MOUSEEVENTF_MOVE;
                    break;
                }
                inputs.push_back(in);
            }
            break;
        case KEYBOARD_EVENT_TYPE: {
                KeyboardEventMsg *kbmsg = (KeyboardEventMsg *)data;
                INPUT in;
                in.type = INPUT_KEYBOARD;
                in.ki.wVk = kbmsg->vkey;
                in.ki.wScan = (kbmsg->state & 0xff0000) >> 16;
                in.ki.dwFlags = 0;
                in.ki.time = 0;
                in.ki.dwExtraInfo = 0;
                if (kbmsg->state & (1 << 31)) {
                    log("[KEYBOARD] event: key_up, wParam %x, lParam %x\n", kbmsg->vkey, kbmsg->state);
                    in.ki.dwFlags |= KEYEVENTF_KEYUP;
                }
                else {
                    log("[KEYBOARD] event: key_down, wParam %x, lParam %x\n", kbmsg->vkey, kbmsg->state);
                }
                if (in.ki.wScan != 0) {
                    in.ki.dwFlags |= KEYEVENTF_SCANCODE;
                }
                if (kbmsg->state & 0x1000000) {
                    in.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
                    in.ki.wScan |= 0xE000;
                }
                inputs.push_back(in);
            }
            break;
        default:
            log("msg format error, unknown msg type %d\n", msgBase->type);
            ret = -2;
        }
        if (ret != 0)
            break;
        data += msgBase->length;
        length -= msgBase->length;
    }
    if (!inputs.empty()) {
        SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
    }
    return ret;
}

static void onError(IIOClient* client, int err) {
    log("transfer error, %d\n", err);
}

int acceptCtrl(IIOClient *ioClient)
{
    g_log = fopen("ctrled.log", "w");
    g_ctrlServer = ioClient;
    IOCallbacks cbs;
    // cbs.on_connect = onConnect;
    cbs.on_data = onData;
    cbs.on_error = onError;
    g_ctrlServer->setCallbacks(std::move(cbs));
    return 0;
}

int stopCtrled()
{
    fclose(g_log);
    g_log = NULL;

    return 0;
}

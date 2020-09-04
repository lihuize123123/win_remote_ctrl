#include "pch.h"
#include "remote_ctrl.h"
#include "ctrl_msg.h"
#include "ctrl_channel.h"
#include <stdio.h>
#include <stdarg.h>
#include <mutex>

HMODULE g_hModule = 0;


static FILE *g_log = NULL;
HHOOK g_mouseProcHook = 0;
HHOOK g_keyboardProcHook = 0;

HWND g_targetWinHwnd = 0;
IIOClient *g_ctrlClient = NULL;

std::mutex g_mutex;
RECT g_displayRect;
//RECT g_desktopRect;
//int g_actualWidth = 0, g_actualHeight = 0;

LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

inline static void log(const char *fmt, ...) {
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    if (g_log != NULL)
        fwrite(buf, 1, strlen(buf), g_log);
}

void setCtrlPanelRect(RECT displayRect)
{
    std::unique_lock<std::mutex> lock(g_mutex);
    g_displayRect = displayRect;
}

/*void setDesktopDemension(RECT desktopRect)
{
    std::unique_lock<std::mutex> lock(g_mutex);
    g_desktopRect = desktopRect;
}*/

int startCtrl(HWND targetWinHwnd, IO_CLIENT_TYPE ctrlChannelType, void *extInfo)
{
    g_log = fopen("ctrl.log", "w");

    g_ctrlClient = createIOClient(ctrlChannelType, extInfo);
    int ret = g_ctrlClient->connect((void *)extInfo);
    if (ret < 0) {
        log("client connect failed, error %d\n", ret);
        return ret;
    }

    g_targetWinHwnd = targetWinHwnd;
    g_mouseProcHook = SetWindowsHookEx(WH_MOUSE,
        mouseProc,
        g_hModule,
        0);
    g_keyboardProcHook = SetWindowsHookEx(WH_KEYBOARD,
        keyboardProc,
        g_hModule,
        0);
    if (g_mouseProcHook == 0 || g_keyboardProcHook == 0) {
        DWORD err = GetLastError();
        log("GetLastError: %d, 0x%x\n", err, err);
        return -10;
    }

    return 0;
}

int stopCtrl()
{
    if (g_mouseProcHook != 0) {
        UnhookWindowsHookEx(g_mouseProcHook);
        g_mouseProcHook = 0;
    }
    if (g_keyboardProcHook != 0) {
        UnhookWindowsHookEx(g_keyboardProcHook);
        g_keyboardProcHook = 0;
    }
    g_ctrlClient->close();
    delete g_ctrlClient;
    g_ctrlClient = NULL;
    fclose(g_log);
    g_log = NULL;
    return 0;
}

int startCtrl1(HWND targetWinHwnd, IIOClient *ioClient)
{
    g_log = fopen("ctrl.log", "w");

    g_ctrlClient = ioClient;
    g_targetWinHwnd = targetWinHwnd;
    g_mouseProcHook = SetWindowsHookEx(WH_MOUSE,
        mouseProc,
        g_hModule,
        0);
    g_keyboardProcHook = SetWindowsHookEx(WH_KEYBOARD,
        keyboardProc,
        g_hModule,
        0);
    if (g_mouseProcHook == 0 || g_keyboardProcHook == 0) {
        DWORD err = GetLastError();
        log("GetLastError: %d, 0x%x\n", err, err);
        return -10;
    }

    return 0;
}

int stopCtrl1()
{
    if (g_mouseProcHook != 0) {
        UnhookWindowsHookEx(g_mouseProcHook);
        g_mouseProcHook = 0;
    }
    if (g_keyboardProcHook != 0) {
        UnhookWindowsHookEx(g_keyboardProcHook);
        g_keyboardProcHook = 0;
    }
    fclose(g_log);
    g_log = NULL;
    return 0;
}

LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // log("mouseProc: nCode %d, wParam %d, lParam %d\n", nCode, wParam, lParam);
    if (g_displayRect.left >= g_displayRect.right ||
        g_displayRect.top >= g_displayRect.bottom/* ||
        g_desktopRect.left >= g_desktopRect.right ||
        g_desktopRect.top >= g_desktopRect.bottom*/) {
        return CallNextHookEx(g_mouseProcHook, nCode, wParam, lParam);
    }

    MOUSEHOOKSTRUCTEX *infoEx = (MOUSEHOOKSTRUCTEX *)lParam;
    if (nCode != HC_ACTION || infoEx->hwnd != g_targetWinHwnd || wParam < WM_MOUSEFIRST || wParam > WM_MOUSELAST) {
        return CallNextHookEx(g_mouseProcHook, nCode, wParam, lParam);
    }

    DWORD extData = 0;
    if (wParam == WM_MOUSEWHEEL) {
        extData = infoEx->mouseData;
    }
#if 0
    log("[MOUSE0] event: %s, x: %d, y: %d, wnd: %d, testCode: %d, extInfo: 0x%p, exData: 0x%x\n",
        getMouseEventName(wParam), infoEx->pt.x, infoEx->pt.y, infoEx->hwnd, infoEx->wHitTestCode, infoEx->dwExtraInfo, extData);
#endif
    MouseEventMsg msg;
    //TODO: convert coordinate
    g_mutex.lock();
    //int deskWidth = g_desktopRect.right - g_desktopRect.left;
    //int deskHeight = g_desktopRect.bottom - g_desktopRect.top;
    int nx = ((infoEx->pt.x - g_displayRect.left) << 16) / (g_displayRect.right - g_displayRect.left);
    int ny = ((infoEx->pt.y - g_displayRect.top) << 16) / (g_displayRect.bottom - g_displayRect.top);
    g_mutex.unlock();
    if (nx < 0 || ny < 0 || nx > 65535 || ny > 65535) {
        return CallNextHookEx(g_mouseProcHook, nCode, wParam, lParam);
    }

    msg.base.type = MOUSE_EVENT_TYPE;
    msg.base.length = sizeof(MouseEventMsg);
    msg.mouse_event = wParam;
    msg.x = nx;
    msg.y = ny;
    msg.extData = extData;

    /*log("pointer(%d, %d), display(%d, %d, %d, %d) desktop(%d, %d, %d, %d)",
        infoEx->pt.x, infoEx->pt.y, g_displayRect.left, g_displayRect.top, g_displayRect.right, g_displayRect.bottom,
        g_desktopRect.left, g_desktopRect.top, g_desktopRect.right, g_desktopRect.bottom);*/
    log("[MOUSE] event: %s, x: %d, y: %d, wnd: %d, exData: 0x%x\n",
        getMouseEventName(msg.mouse_event), msg.x, msg.y, infoEx->hwnd, msg.extData);
    //TODO: send udp msg
    g_ctrlClient->sendmsg((char *)&msg, sizeof(msg));
    return CallNextHookEx(g_mouseProcHook, nCode, wParam, lParam);
}

LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // log("keyboardProc: nCode %d, wParam %x, lParam %x, fgWnd %d, targetWnd %d\n", nCode, wParam, lParam, GetForegroundWindow(), g_targetWinHwnd);
    // MOUSEHOOKSTRUCTEX *infoEx = (MOUSEHOOKSTRUCTEX *)lParam;
    if (nCode != HC_ACTION || GetForegroundWindow() != g_targetWinHwnd) {
        return CallNextHookEx(g_keyboardProcHook, nCode, wParam, lParam);
    }

    if (lParam & (1 << 31)) {
        log("[KEYBOARD] event: key_up, wParam %x, lParam %x\n", wParam, lParam);
    }
    else {
        log("[KEYBOARD] event: key_down, wParam %x, lParam %x\n", wParam, lParam);
    }
    KeyboardEventMsg msg;
    msg.base.type = KEYBOARD_EVENT_TYPE;
    msg.base.length = sizeof(KeyboardEventMsg);
    msg.vkey = wParam;
    msg.state = lParam;
    // TODO: send udp msg
    g_ctrlClient->sendmsg((char *)&msg, sizeof(msg));
    return CallNextHookEx(g_keyboardProcHook, nCode, wParam, lParam);
}


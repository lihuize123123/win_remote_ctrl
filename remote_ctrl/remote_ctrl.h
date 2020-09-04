#pragma once
#include <windows.h>
#include <string>
#include "ctrl_channel.h"

#ifdef REMOTE_CTRL_EXPORTS
#define REMOTE_CTRL_API __declspec(dllexport)
#else
#define REMOTE_CTRL_API __declspec(dllimport)
#endif

REMOTE_CTRL_API void setCtrlPanelRect(RECT displayRect);
//REMOTE_CTRL_API void setDesktopDemension(RECT desktopRect);
//REMOTE_CTRL_API int startCtrl(HWND ctrlPanel, IO_CLIENT_TYPE ctrlChannelType, void *extInfo);
REMOTE_CTRL_API int startCtrl1(HWND ctrlPanel, IIOClient *ioClient);
//REMOTE_CTRL_API int stopCtrl();
REMOTE_CTRL_API int stopCtrl1();
//REMOTE_CTRL_API int acceptCtrl(IO_CLIENT_TYPE ctrlChannelType, void *extInfo);
REMOTE_CTRL_API int acceptCtrl1(IIOClient * ioClient);
//REMOTE_CTRL_API int stopCtrled();
REMOTE_CTRL_API int stopCtrled1();

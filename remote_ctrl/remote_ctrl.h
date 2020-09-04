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
REMOTE_CTRL_API int startCtrl(HWND ctrlPanel, IIOClient *ioClient);
REMOTE_CTRL_API int stopCtrl();
REMOTE_CTRL_API int acceptCtrl(IIOClient * ioClient);
REMOTE_CTRL_API int stopCtrled();

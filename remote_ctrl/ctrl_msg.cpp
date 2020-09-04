#include "ctrl_msg.h"
#include <map>

const char *getMouseEventName(int evt) {
    std::map<int, const char *> msgTable = {
        {WM_MOUSEMOVE, "mouse_move"},
        {WM_LBUTTONDOWN, "lb_down"},
        {WM_LBUTTONUP, "lb_up"},
        {WM_LBUTTONDBLCLK, "lb_dbl_clk"},
        {WM_RBUTTONDOWN, "rb_down"},
        {WM_RBUTTONUP, "rb_up"},
        {WM_RBUTTONDBLCLK, "rb_dbl_clk"},
        {WM_MBUTTONDOWN, "mb_down"},
        {WM_MBUTTONUP, "mb_up"},
        {WM_MBUTTONDBLCLK, "mb_dbl_clk"},
        {WM_MOUSEWHEEL, "mouse_wheel"},
        {WM_MOUSEHWHEEL, "mouse_hwheel"},
        {WM_XBUTTONDOWN, "xb_down"},
        {WM_XBUTTONUP, "xb_up"},
        {WM_XBUTTONDBLCLK, "xb_dbl_clk"},
    };
    auto iter = msgTable.find(evt);
    if (iter != msgTable.end()) {
        return iter->second;
    }
    return "unknown";
}

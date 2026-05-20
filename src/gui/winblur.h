// SPDX-License-Identifier: GPL-3.0-or-later
// Shared Win11 DWM acrylic/blur utility.
// Include this header only from .cpp files (not from other headers).

#pragma once

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#  define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#  define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

namespace CopyQWin11 {

#define COPYQ_WCA_ACCENT_POLICY 19

struct AccentPolicy {
    DWORD AccentState;
    DWORD AccentFlags;
    DWORD GradientColor; // AABBGGRR
    DWORD AnimationId;
};

struct WcaData {
    DWORD Attribute;
    PVOID pData;
    ULONG cbData;
};

using SetWindowCompositionAttributeFn = BOOL(WINAPI*)(HWND, WcaData*);

inline void applyBlur(HWND hwnd)
{
    // Extend DWM frame over entire client area so DWM composites through Qt.
    const MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Enable blur-behind (still works on Win11).
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(hwnd, &bb);

    // Also try SetWindowCompositionAttribute (Win10 1803+, stronger Acrylic effect).
    const auto user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        return;
    const auto fn = reinterpret_cast<SetWindowCompositionAttributeFn>(
        GetProcAddress(user32, "SetWindowCompositionAttribute"));
    if (!fn)
        return;

    AccentPolicy policy{3, 0, 0x00000000, 0};
    WcaData data{COPYQ_WCA_ACCENT_POLICY, &policy, sizeof(policy)};
    fn(hwnd, &data);
}

} // namespace CopyQWin11
#endif // Q_OS_WIN

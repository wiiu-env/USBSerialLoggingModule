#include <wums.h>
#include <coreinit/ios.h>
#include <coreinit/debug.h>
#include <coreinit/cache.h>
#include <kernel/kernel.h>
#include <whb/log_udp.h>
#include "kernel.h"

WUMS_MODULE_EXPORT_NAME("homebrew_usbseriallogging");
WUMS_MODULE_SKIP_ENTRYPOINT();

extern "C" void SC_0x51();

WUMS_INITIALIZE() {
    WHBLogUdpInit();

    // Start syslogging on iosu side
    int mcpFd = IOS_Open("/dev/mcp", (IOSOpenMode) 0);
    if (mcpFd >= 0) {
        int in = 0xFA; // IPC_CUSTOM_START_USB_LOGGING
        int out = 0;
        IOS_Ioctl(mcpFd, 100, &in, sizeof(in), &out, sizeof(out));
        IOS_Close(mcpFd);
    }

    // Patch loader.elf to spit out less warnings when loading .rpx built with wut
    KernelNOPAtPhysicalAddress(0x0100b770 - 0x01000000 + 0x32000000);
    KernelNOPAtPhysicalAddress(0x0100b800 - 0x01000000 + 0x32000000);
    KernelNOPAtPhysicalAddress(0x0100b7b8 - 0x01000000 + 0x32000000);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b770), 0x04);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b800), 0x04);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b7b8), 0x04);

    KernelPatchSyscall(0x51, (uint32_t) &IopShellInitInternal);

    // Start iopshell on kernel
    SC_0x51();
}

#define IopShell_UserCallback (0x101C400 + 0x1926c)
#define IopShell_RegisterCallback ((void (*)( uint32_t,uint32_t,uint32_t,uint32_t))(0x101C400 + 0x19638))
#define IopShell_CreateThread ((void (*)( void))(0x101C400 + 0x19504))

WUMS_APPLICATION_STARTS(){
    IopShell_RegisterCallback(IopShell_UserCallback, 0x100978f8, 0x10097900, 0x10097c40);
    IopShell_CreateThread();
}


#include "kernel.h"
#include "logger.h"
#include <coreinit/cache.h>
#include <coreinit/ios.h>
#include <kernel/kernel.h>
#include <mocha/mocha.h>
#include <wums.h>

WUMS_MODULE_EXPORT_NAME("homebrew_usbseriallogging");
WUMS_MODULE_SKIP_INIT_FINI();
WUMS_DEPENDS_ON(homebrew_kernel);

extern "C" void SC_0x51();

WUMS_INITIALIZE() {
    initLogging();

    int res;
    if ((res = Mocha_InitLibrary()) != MOCHA_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Mocha_InitLibrary() failed %d", res);
        return;
    }

    if (Mocha_StartUSBLogging(false) != MOCHA_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Mocha_StartUSBLogging failed");
        return;
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
    deinitLogging();
}

#define IopShell_UserCallback     (0x101C400 + 0x1926c)
#define IopShell_RegisterCallback ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))(0x101C400 + 0x19638))
#define IopShell_CreateThread     ((void (*)(void))(0x101C400 + 0x19504))

WUMS_APPLICATION_STARTS() {
    initLogging();
    DEBUG_FUNCTION_LINE("Register IopShell_UserCallback");
    IopShell_RegisterCallback(IopShell_UserCallback, 0x100978f8, 0x10097900, 0x10097c40);
    DEBUG_FUNCTION_LINE("IopShell create thread");
    IopShell_CreateThread();
    deinitLogging();
}

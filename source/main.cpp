#include "logger.h"
#include "version.h"
#include <coreinit/cache.h>
#include <coreinit/ios.h>
#include <kernel/kernel.h>
#include <mocha/mocha.h>
#include <wums.h>

WUMS_MODULE_EXPORT_NAME("homebrew_usbseriallogging");
WUMS_MODULE_SKIP_INIT_FINI();
WUMS_DEPENDS_ON(homebrew_kernel);

#define MODULE_VERSION "v0.1.2"

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

    deinitLogging();
}

WUMS_APPLICATION_STARTS() {
    OSReport("Running USBSerialLoggingModule " MODULE_VERSION MODULE_VERSION_EXTRA "\n");
}

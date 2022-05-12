#include <cstdint>

#pragma GCC push_options
#pragma GCC optimize("O0")

#define k_memset               ((void (*)(uint32_t, uint32_t, uint32_t)) 0xfff09d60)
#define KiReport               ((void (*)(const char *, ...)) 0xfff0ad0c)
#define IopShell_AsyncCallback (0xfff1b7d8)
#define IopShell_ReadCallback  (0xfff1b9a0)
#define k_memcpy               ((void (*)(void *, void *, uint32_t)) 0xfff09e44)
#define IPCKDriver_OpenAsync   ((int32_t(*)(uint32_t client_ram_pid, const char *device, uint32_t mode, uint32_t asyncCallback, void *context)) 0xfff0b9c8)
#define WaitAsyncReply         ((uint32_t(*)(void *, uint32_t, uint32_t)) 0xfff180dc)

#define IPCKDriver_IoctlAsync  ((int32_t(*)( \
        uint32_t clientProcessId,            \
        uint32_t loaderProcessId,            \
        uint32_t handle,                     \
        uint32_t request,                    \
        void *inBuf,                         \
        uint32_t inLen,                      \
        void *outBuf,                        \
        uint32_t outLen,                     \
        uint32_t asyncCallback,              \
        void *context)) 0xfff0bc5c)


uint32_t IPCKDriver_IOS_Open_Sync(uint32_t clientProcessId, const char *device_name, uint32_t mode) {
    register int r13 asm("r13");
    k_memcpy((void *) (r13 - 0x1180), (void *) device_name, 0x10);
    void *context = (void *) (r13 + 0x450);
    auto error    = IPCKDriver_OpenAsync(clientProcessId, (const char *) (r13 - 0x1180), mode, IopShell_AsyncCallback, context);
    if (error < 0) {
        return error;
    }
    return WaitAsyncReply(context, 20000000, 6);
}

uint32_t IPCKDriver_IOS_Ioctl_Sync(uint32_t clientProcessId, uint32_t handle, uint32_t request, void *inBuf, uint32_t inLen, void *outBuf, uint32_t ouLen) {
    register int r13 asm("r13");
    void *context = (void *) (r13 + 0x450);

    auto error = IPCKDriver_IoctlAsync(clientProcessId, 0xFFFFFFFF, handle, request, inBuf, inLen, outBuf, ouLen, IopShell_AsyncCallback, context);

    if (error < 0) {
        return error;
    }
    uint32_t res = WaitAsyncReply(context, 20000000, 6);
    return res;
}


void IopShellInitInternal() {
    register int r13 asm("r13");
    *((uint32_t *) (r13 + 0x450)) = 0;
    *((uint32_t *) (r13 + 0x454)) = 0;
    *((uint32_t *) (r13 + 0x458)) = 0xffffffe3;
    auto handle                   = IPCKDriver_IOS_Open_Sync(0, "/dev/iopsh", (uint32_t) 5);
    if (static_cast<int>(handle) < 0) {
        KiReport("####################################################################################\n");
        KiReport("IopShellInit failed\n");
        KiReport("####################################################################################\n");
        return;
    }
    *(uint32_t *) (r13 + -0x6d84) = static_cast<uint32_t>(handle);
    *(uint32_t *) (r13 + -0x1178) = 5;
    *(uint32_t *) (r13 + -0x117c) = 0x1040;

    auto result = IPCKDriver_IOS_Ioctl_Sync(0, static_cast<uint32_t>(handle), 3, (void *) (r13 - 0x1180), 0x14, (void *) (r13 - 0x1140), 0x4);
    if (static_cast<int>(result) < 0) {
        KiReport("####################################################################################\n");
        KiReport("IopShellInit failed\n");
        KiReport("####################################################################################\n");
        return;
    }
    *(uint32_t *) (r13 + -0x117c) = 0x636F7300; // "cos"
    result                        = IPCKDriver_IOS_Ioctl_Sync(0, static_cast<uint32_t>(handle), 1, (void *) (r13 - 0x1180), 0x14, (void *) (r13 - 0x1140), 0x4);
    if (static_cast<int>(result) < 0) {
        KiReport("####################################################################################\n");
        KiReport("IopShellInit failed\n");
        KiReport("####################################################################################\n");
        return;
    }
    *((uint32_t *) (r13 + 0x450)) = 0;
    *((uint32_t *) (r13 + 0x454)) = 0;
    *((uint32_t *) (r13 + 0x458)) = 0xffffffe3;
    k_memset((r13 - 0x1180), 0, 0x1040);
    result = IPCKDriver_IoctlAsync(0, 0xFFFFFFFF, handle, 5, nullptr, 0, (void *) (r13 - 0x1180), 0x1040, IopShell_ReadCallback, (void *) (r13 + 0x450));
    if (static_cast<int>(result) < 0) {
        KiReport("####################################################################################\n");
        KiReport("IopShellInit failed\n");
        KiReport("####################################################################################\n");
        return;
    }
    KiReport("IopShellInit was successful\n");
}
#pragma GCC pop_options
#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/suspend.h>
#include <taihen.h>
#include <string.h>

int ksceKernelSysrootCheckModelCapability(int cap);
int module_get_export_func(SceUID pid, const char *modname, uint32_t libnid, uint32_t funcnid, uintptr_t *func);

static SceUID g_hook = 0;
static tai_hook_ref_t g_ksceKernelSysrootCheckModelCapability_hook;

int (*sceUsbServPortModeSetForDriver)(SceUInt32 usbPort, SceBool clientMode);
SceBool (*sceUsbServPortModeGetForDriver)(SceUInt32 usbPort);

static int eth_sysevent_handler(int resume, int eventid, void *args, void *opt) {
    if (resume)
    {
        sceUsbServPortModeSetForDriver(2, 0); // re-set host mode
    }
    return 0;
}

static int ksceKernelSysrootCheckModelCapability_patched(int cap) {
    int ret = TAI_CONTINUE(int, g_ksceKernelSysrootCheckModelCapability_hook, cap);
    // TODO: check set caps on devkit/testkit. force only 5 for now
    if (cap == 5)// || cap == 6)
    {
        ret = 1;
    }
    return ret;
}

int module_start(int args, void *argv) {
    (void)args;
    (void)argv;
    int ret;

    module_get_export_func(KERNEL_PID, "SceUsbServ", 0xA75BBDF2, 0x7AD36284, (uintptr_t *)&sceUsbServPortModeSetForDriver);
    module_get_export_func(KERNEL_PID, "SceUsbServ", 0xA75BBDF2, 0xF0553A69, (uintptr_t *)&sceUsbServPortModeGetForDriver);

    // settings check paf::system::SupportsWiredEthernet(), which check capability 5 and 6, so fake one
    g_hook = taiHookFunctionExportForKernel(KERNEL_PID, &g_ksceKernelSysrootCheckModelCapability_hook, "SceSysmem", 0x2ED7F97A, 0x8AA268D6, ksceKernelSysrootCheckModelCapability_patched);
    ksceDebugPrintf("taiHookFunctionExportForKernel: 0x%08X\n", g_hook);

    // enable host mode (0) on multicn (port 2)
    ret = sceUsbServPortModeSetForDriver(2, 0);
    ksceDebugPrintf("Port: 2, result: %x \n", ret);

    int cap = 0;

    cap = (ksceKernelSysrootCheckModelCapability(5) || ksceKernelSysrootCheckModelCapability(6));
    if (!cap)
    {
        g_hook = taiHookFunctionExportForKernel(KERNEL_PID, &g_ksceKernelSysrootCheckModelCapability_hook, "SceSysmem", 0x2ED7F97A, 0x8AA268D6, ksceKernelSysrootCheckModelCapability_patched);
        ksceDebugPrintf("taiHookFunctionExportForKernel: 0x%08X\n", g_hook);
    }

    // to restore host on resume
    ksceKernelRegisterSysEventHandler("zeth_sysevent", eth_sysevent_handler, NULL);

    return SCE_KERNEL_START_SUCCESS;
}
void _start() __attribute__ ((weak, alias ("module_start")));

int module_stop()
{
    if (g_hook > 0)
    {
        taiHookReleaseForKernel(g_hook, g_ksceKernelSysrootCheckModelCapability_hook);
    }
    return SCE_KERNEL_STOP_SUCCESS;
}

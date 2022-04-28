#include "sdutils/sdutils.h"
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <dirent.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static SDUtilsVersion (*sSDUtilsGetVersion)() = nullptr;

static bool (*sSDUtilsAddAttachHandler)(SDAttachHandlerFn)    = nullptr;
static bool (*sSDUtilsRemoveAttachHandler)(SDAttachHandlerFn) = nullptr;

SDUtilsStatus SDUtils_Init() {
    if (OSDynLoad_Acquire("homebrew_sdhotswap", &sModuleHandle) != OS_DYNLOAD_OK) {
        OSReport("SDUtils_Init: OSDynLoad_Acquire failed.\n");
        return SDUTILS_RESULT_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsGetVersion", (void **) &sSDUtilsGetVersion) != OS_DYNLOAD_OK) {
        OSReport("SDUtils_Init: SDUtilsGetVersion failed.\n");
        return SDUTILS_RESULT_MODULE_MISSING_EXPORT;
    }
    auto res = SDUtils_GetVersion();
    if (res != SDUTILS_MODULE_VERSION) {
        return SDUTILS_RESULT_UNSUPPORTED_VERSION;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsAddAttachHandler", (void **) &sSDUtilsAddAttachHandler) != OS_DYNLOAD_OK) {
        OSReport("SDUtils_Init: SDUtilsAddAttachHandler failed.\n");
        return SDUTILS_RESULT_MODULE_MISSING_EXPORT;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsRemoveAttachHandler", (void **) &sSDUtilsRemoveAttachHandler) != OS_DYNLOAD_OK) {
        OSReport("SDUtils_Init: SDUtilsRemoveAttachHandler failed.\n");
        return SDUTILS_RESULT_MODULE_MISSING_EXPORT;
    }

    return SDUTILS_RESULT_SUCCESS;
}

SDUtilsVersion GetVersion();
SDUtilsVersion SDUtils_GetVersion() {
    if (sSDUtilsGetVersion == nullptr) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }

    return reinterpret_cast<decltype(&GetVersion)>(sSDUtilsGetVersion)();
}

SDUtilsStatus SDUtils_DeInit() {
    // We don't need to release the OSDynLoad handle for modules.
    return SDUTILS_RESULT_SUCCESS;
}


SDUtilsStatus SDUtils_IsSdCardMounted(bool *status) {
    if (status == nullptr) {
        return SDUTILS_RESULT_INVALID_ARGUMENT;
    }
    auto dir = opendir("fs:/vol/external01/");
    if (dir != nullptr) {
        closedir(dir);
        *status = true;
    } else {
        *status = false;
    }
    return SDUTILS_RESULT_SUCCESS;
}

bool AddAttachHandler(SDAttachHandlerFn);

SDUtilsStatus SDUtils_AddAttachHandler(SDAttachHandlerFn fn) {
    if (sSDUtilsAddAttachHandler == nullptr) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }
    auto res = reinterpret_cast<decltype(&AddAttachHandler)>(sSDUtilsAddAttachHandler)(fn);
    return res ? SDUTILS_RESULT_SUCCESS : SDUTILS_RESULT_MAX_CALLBACKS;
}


bool RemoveAttachHandler(SDAttachHandlerFn);

SDUtilsStatus SDUtils_RemoveAttachHandler(SDAttachHandlerFn fn) {
    if (sSDUtilsRemoveAttachHandler == nullptr) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }
    auto res = reinterpret_cast<decltype(&RemoveAttachHandler)>(sSDUtilsRemoveAttachHandler)(fn);
    return res ? SDUTILS_RESULT_SUCCESS : SDUTILS_RESULT_NOT_FOUND;
}
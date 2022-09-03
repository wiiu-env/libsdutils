#include "logger.h"
#include "sdutils/sdutils.h"
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <dirent.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static SDUtilsStatus (*sSDUtilsGetVersion)(SDUtilsVersion *) = nullptr;

static bool (*sSDUtilsAddAttachHandler)(SDAttachHandlerFn)    = nullptr;
static bool (*sSDUtilsRemoveAttachHandler)(SDAttachHandlerFn) = nullptr;

static bool (*sSDUtilsAddCleanUpHandlesHandler)(SDCleanUpHandlesHandlerFn)    = nullptr;
static bool (*sSDUtilsRemoveCleanUpHandlesHandler)(SDCleanUpHandlesHandlerFn) = nullptr;

static SDUtilsVersion sSDUtilsVersion = SD_UTILS_MODULE_VERSION_ERROR;

SDUtilsStatus SDUtils_InitLibrary() {
    if (OSDynLoad_Acquire("homebrew_sdhotswap", &sModuleHandle) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("OSDynLoad_Acquire homebrew_sdhotswap failed.");
        return SDUTILS_RESULT_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsGetVersion", (void **) &sSDUtilsGetVersion) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport SDUtilsGetVersion failed.");
        return SDUTILS_RESULT_MODULE_MISSING_EXPORT;
    }

    auto res = SDUtils_GetVersion(&sSDUtilsVersion);
    if (res != SDUTILS_RESULT_SUCCESS) {
        return SDUTILS_RESULT_UNSUPPORTED_VERSION;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsAddAttachHandler", (void **) &sSDUtilsAddAttachHandler) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport SDUtilsAddAttachHandler failed.");
        sSDUtilsAddAttachHandler = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsRemoveAttachHandler", (void **) &sSDUtilsRemoveAttachHandler) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport SDUtilsRemoveAttachHandler failed.");
        sSDUtilsRemoveAttachHandler = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsAddCleanUpHandlesHandler", (void **) &sSDUtilsAddCleanUpHandlesHandler) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport SDUtilsAddCleanUpHandlesHandler failed.");
        sSDUtilsAddCleanUpHandlesHandler = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsRemoveCleanUpHandlesHandler", (void **) &sSDUtilsRemoveCleanUpHandlesHandler) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport SDUtilsRemoveCleanUpHandlesHandler failed.");
        sSDUtilsRemoveCleanUpHandlesHandler = nullptr;
    }

    return SDUTILS_RESULT_SUCCESS;
}

SDUtilsStatus SDUtils_DeInitLibrary() {
    // We don't need to release the OSDynLoad handle for modules.
    return SDUTILS_RESULT_SUCCESS;
}

SDUtilsStatus GetVersion(SDUtilsVersion *);
SDUtilsStatus SDUtils_GetVersion(SDUtilsVersion *outVersion) {
    if (sSDUtilsGetVersion == nullptr) {
        if (OSDynLoad_Acquire("homebrew_sdhotswap", &sModuleHandle) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("OSDynLoad_Acquire failed.");
            return SDUTILS_RESULT_MODULE_NOT_FOUND;
        }

        if (OSDynLoad_FindExport(sModuleHandle, FALSE, "SDUtilsGetVersion", (void **) &sSDUtilsGetVersion) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("FindExport SDUtilsGetVersion failed.");
            return SDUTILS_RESULT_MODULE_MISSING_EXPORT;
        }
    }
    if (outVersion == nullptr) {
        return SDUTILS_RESULT_INVALID_ARGUMENT;
    }
    return reinterpret_cast<decltype(&GetVersion)>(sSDUtilsGetVersion)(outVersion);
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
    if (sSDUtilsVersion == SD_UTILS_MODULE_VERSION_ERROR) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }
    if (sSDUtilsAddAttachHandler == nullptr || sSDUtilsVersion < 1) {
        return SDUTILS_RESULT_UNSUPPORTED_COMMAND;
    }
    auto res = reinterpret_cast<decltype(&AddAttachHandler)>(sSDUtilsAddAttachHandler)(fn);
    return res ? SDUTILS_RESULT_SUCCESS : SDUTILS_RESULT_MAX_CALLBACKS;
}

bool RemoveAttachHandler(SDAttachHandlerFn);

SDUtilsStatus SDUtils_RemoveAttachHandler(SDAttachHandlerFn fn) {
    if (sSDUtilsVersion == SD_UTILS_MODULE_VERSION_ERROR) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }
    if (sSDUtilsRemoveAttachHandler == nullptr || sSDUtilsVersion < 1) {
        return SDUTILS_RESULT_UNSUPPORTED_COMMAND;
    }
    auto res = reinterpret_cast<decltype(&RemoveAttachHandler)>(sSDUtilsRemoveAttachHandler)(fn);
    return res ? SDUTILS_RESULT_SUCCESS : SDUTILS_RESULT_NOT_FOUND;
}

bool AddCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn);

SDUtilsStatus SDUtils_AddCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn fn) {
    if (sSDUtilsVersion == SD_UTILS_MODULE_VERSION_ERROR) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }
    if (sSDUtilsAddCleanUpHandlesHandler == nullptr || sSDUtilsVersion < 1) {
        return SDUTILS_RESULT_UNSUPPORTED_COMMAND;
    }
    auto res = reinterpret_cast<decltype(&AddCleanUpHandlesHandler)>(sSDUtilsAddCleanUpHandlesHandler)(fn);
    return res ? SDUTILS_RESULT_SUCCESS : SDUTILS_RESULT_MAX_CALLBACKS;
}

bool RemoveCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn);

SDUtilsStatus SDUtils_RemoveCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn fn) {
    if (sSDUtilsVersion == SD_UTILS_MODULE_VERSION_ERROR) {
        return SDUTILS_RESULT_LIB_UNINITIALIZED;
    }
    if (sSDUtilsRemoveCleanUpHandlesHandler == nullptr || sSDUtilsVersion < 1) {
        return SDUTILS_RESULT_UNSUPPORTED_COMMAND;
    }
    auto res = reinterpret_cast<decltype(&RemoveCleanUpHandlesHandler)>(sSDUtilsRemoveCleanUpHandlesHandler)(fn);
    return res ? SDUTILS_RESULT_SUCCESS : SDUTILS_RESULT_NOT_FOUND;
}
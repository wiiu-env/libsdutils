#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum SDUtilsStatus {
    SDUTILS_RESULT_SUCCESS               = 0,
    SDUTILS_RESULT_MODULE_NOT_FOUND      = -0x1,
    SDUTILS_RESULT_MODULE_MISSING_EXPORT = -0x2,
    SDUTILS_RESULT_MAX_CALLBACKS         = -0x3,
    SDUTILS_RESULT_NOT_FOUND             = -0x4,
    SDUTILS_RESULT_INVALID_ARGUMENT      = -0x5,
    SDUTILS_RESULT_FAILED                = -0x10,
    SDUTILS_RESULT_LIB_UNINITIALIZED     = -0x20,
    SDUTILS_RESULT_UNSUPPORTED_VERSION   = -0x99,
    SDUTILS_RESULT_UNSUPPORTED_COMMAND   = -0x100,
    SDUTILS_RESULT_UNKNOWN_ERROR         = -0x1000,
} SDUtilsStatus;

typedef uint32_t SDUtilsVersion;
#define SD_UTILS_MODULE_VERSION_ERROR 0xFFFFFFFF

typedef enum SDUtilsAttachStatus {
    SDUTILS_ATTACH_MOUNTED   = 1,
    SDUTILS_ATTACH_UNMOUNTED = 0,
} SDUtilsAttachStatus;

typedef void (*SDAttachHandlerFn)(SDUtilsAttachStatus status);

typedef void (*SDCleanUpHandlesHandlerFn)();

/**
 * Initializes the SDUtils library. This must be call before any other function can be called
 * @return SDUTILS_RESULT_SUCCESS on success, the functions of this lib can be used <br>
 *         SDUTILS_RESULT_MODULE_NOT_FOUND when the SDHotSwapModule is not loaded <br>
 *         SDUTILS_RESULT_MODULE_MISSING_EXPORT when the SDHotSwapModule does not export the expected functions.
 */
SDUtilsStatus SDUtils_InitLibrary();

/**
 * Deinitializes the SDUtils library, must be called before exiting the application
 * @return SDUTILS_RESULT_SUCCESS on success
 */
SDUtilsStatus SDUtils_DeInitLibrary();

/**
 * Retrieves the API Version of the loaded SDUtils.<br>
 * <br>
 * @param outVersion pointer to the variable where the version will be stored.
 *
 * @return SDUTILS_RESULT_SUCCESS:               The API version has been store in the version ptr.<br>
 *         SDUTILS_RESULT_MODULE_NOT_FOUND:      The module could not be found. Make sure the module is loaded.<br>
 *         SDUTILS_RESULT_MODULE_MISSING_EXPORT: The module is missing an expected export.<br>
 *         SDUTILS_RESULT_INVALID_ARGUMENT:      Invalid version pointer.<br>
 *         SDUTILS_RESULT_UNKNOWN_ERROR:         Retrieving the module version failed.
 */
SDUtilsStatus SDUtils_GetVersion(SDUtilsVersion *outVersion);

/**
 * Registers a callback which will be called whenever a sd card will be inserted or ejected. <br>
 * This is only true for future events, if the sd card is already inserted before registering <br>
 * a callback, the callback is only called on the next ejecting but not for the initial <br>
 * insertion that happened in the past. <br>
 * <br>
 * Any previously registered callbacks will be removed when the currently running application <br>
 * is closing. <br>
 *
 * @param fn callback that will be called
 * @return SDUTILS_RESULT_SUCCESS on success <br>
 *         SDUTILS_RESULT_MAX_CALLBACKS         when registering the callback has failed because the <br>
 *                                              maximum amount of callback has been reached <br>
 *         SDUTILS_RESULT_LIB_UNINITIALIZED     if the lib was not initialized properly <br>
 *         SDUTILS_RESULT_UNSUPPORTED_COMMAND   Command not supported by the currently running module
 */
SDUtilsStatus SDUtils_AddAttachHandler(SDAttachHandlerFn fn);

/**
 * Removed a previously registered callback
 *
 * @param fn
 * @return SDUTILS_RESULT_SUCCESS               on success <br>
 *         SDUTILS_RESULT_NOT_FOUND             when the given callback was not registered. <br>
 *         SDUTILS_RESULT_LIB_UNINITIALIZED     if the lib was not initialized properly <br>
 *         SDUTILS_RESULT_UNSUPPORTED_COMMAND   Command not supported by the currently running module
 */
SDUtilsStatus SDUtils_RemoveAttachHandler(SDAttachHandlerFn fn);

/**
 * Registers a callback which will be called whenever a sd card will be ejected and before <br>
 * the SDUtils_AddAttachHandler. This callback is supposed to be used to clean up any open <br>
 * file handles before the sd card gets unmounted. <br>
 * This is only true for future events, if the sd card is already ejected before registering <br>
 * a callback, the callback is only called on the next ejecting. <br>
 * <br>
 * Any previously registered callbacks will be removed when the currently running application <br>
 * is closing. <br>
 *
 * @param fn callback that will be called
 * @return SDUTILS_RESULT_SUCCESS on success <br>
 *         SDUTILS_RESULT_MAX_CALLBACKS         when registering the callback has failed because the <br>
 *                                              maximum amount of callback has been reached <br>
 *         SDUTILS_RESULT_LIB_UNINITIALIZED     if the lib was not initialized properly <br>
 *         SDUTILS_RESULT_UNSUPPORTED_COMMAND   Command not supported by the currently running module
 */
SDUtilsStatus SDUtils_AddCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn fn);

/**
 * Removed a previously registered callback
 *
 * @param fn
 * @return SDUTILS_RESULT_SUCCESS               on success <br>
 *         SDUTILS_RESULT_NOT_FOUND             when the given callback was not registered. <br>
 *         SDUTILS_RESULT_LIB_UNINITIALIZED     if the lib was not initialized properly <br>
 *         SDUTILS_RESULT_UNSUPPORTED_COMMAND   Command not supported by the currently running module
 */
SDUtilsStatus SDUtils_RemoveCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn fn);

/**
 * Checks if a FAT32 formatted SD Card is inserted, mounted and available via `fs:/vol/external01`
 *
 * @param outStatus stores the result of the function
 * @return SDUTILS_RESULT_SUCCESS  on success  <br>
 *         SDUTILS_RESULT_INVALID_ARGUMENT when an invalid ptr has been given
 */
SDUtilsStatus SDUtils_IsSdCardMounted(bool *outStatus);

#ifdef __cplusplus
} // extern "C"
#endif
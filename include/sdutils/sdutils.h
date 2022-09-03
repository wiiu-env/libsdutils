#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum SDUtilsStatus {
    SDUTILS_RESULT_SUCCESS               = 0,
    SDUTILS_RESULT_MODULE_NOT_FOUND      = -1,
    SDUTILS_RESULT_MODULE_MISSING_EXPORT = -2,
    SDUTILS_RESULT_MAX_CALLBACKS         = -3,
    SDUTILS_RESULT_NOT_FOUND             = -4,
    SDUTILS_RESULT_INVALID_ARGUMENT      = -5,
    SDUTILS_RESULT_FAILED                = -10,
    SDUTILS_RESULT_LIB_UNINITIALIZED     = -20,
    SDUTILS_RESULT_UNSUPPORTED_VERSION   = -99,
};

typedef uint32_t SDUtilsVersion;
#define SD_UTILS_MODULE_VERSION_ERROR 0xFFFFFFFF

enum SDUtilsAttachStatus {
    SDUTILS_ATTACH_MOUNTED   = 1,
    SDUTILS_ATTACH_UNMOUNTED = 0,
};

typedef void (*SDAttachHandlerFn)(SDUtilsAttachStatus status);

typedef void (*SDCleanUpHandlesHandlerFn)();

/**
 * Initializes the SDUtils library. This must be call before any other function can be called
 * @return SDUTILS_RESULT_SUCCESS on success, the functions of this lib can be used
 *         SDUTILS_RESULT_MODULE_NOT_FOUND when the SDHotSwapModule is not loaded
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
 * Registers a callback which will be called whenever a sd card will be inserted or ejected.
 * This is only true for future events, if the sd card is already inserted before registering
 * a callback, the callback is only called on the next ejecting but not for the initial
 * insertion that happened in the past.
 *
 * Any previously registered callbacks will be removed when the currently running application
 * is closing.
 *
 * @param fn callback that will be called
 * @return SDUTILS_RESULT_SUCCESS on success
 *         SDUTILS_RESULT_MAX_CALLBACKS when registering the callback has failed because the
 *                                      maximum amount of callback has been reached
 *         SDUTILS_RESULT_LIB_UNINITIALIZED if the lib was not initialized properly
 */
SDUtilsStatus SDUtils_AddAttachHandler(SDAttachHandlerFn fn);

/**
 * Removed a previously registered callback
 *
 * @param fn
 * @return SDUTILS_RESULT_SUCCESS on success
 *         SDUTILS_RESULT_NOT_FOUND when the given callback was not registered.
 *         SDUTILS_RESULT_LIB_UNINITIALIZED if the lib was not initialized properly
 */
SDUtilsStatus SDUtils_RemoveAttachHandler(SDAttachHandlerFn fn);

/**
 * Registers a callback which will be called whenever a sd card will be ejected and before
 * the SDUtils_AddAttachHandler. This callback is supposed to be used to clean up any open
 * file handles before the sd card gets unmounted.
 * This is only true for future events, if the sd card is already ejected before registering
 * a callback, the callback is only called on the next ejecting.
 *
 * Any previously registered callbacks will be removed when the currently running application
 * is closing.
 *
 * @param fn callback that will be called
 * @return SDUTILS_RESULT_SUCCESS on success
 *         SDUTILS_RESULT_MAX_CALLBACKS when registering the callback has failed because the
 *                                      maximum amount of callback has been reached
 *         SDUTILS_RESULT_LIB_UNINITIALIZED if the lib was not initialized properly
 */
SDUtilsStatus SDUtils_AddCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn fn);

/**
 * Removed a previously registered callback
 *
 * @param fn
 * @return SDUTILS_RESULT_SUCCESS on success
 *         SDUTILS_RESULT_NOT_FOUND when the given callback was not registered.
 *         SDUTILS_RESULT_LIB_UNINITIALIZED if the lib was not initialized properly
 */
SDUtilsStatus SDUtils_RemoveCleanUpHandlesHandler(SDCleanUpHandlesHandlerFn fn);

/**
 * Checks if a FAT32 formatted SD Card is inserted, mounted and available via `fs:/vol/external01`
 *
 * @param outStatus stores the result of the function
 * @return SDUTILS_RESULT_SUCCESS  on success
 *         SDUTILS_RESULT_INVALID_ARGUMENT when an invalid ptr has been given
 */
SDUtilsStatus SDUtils_IsSdCardMounted(bool *outStatus);

#ifdef __cplusplus
} // extern "C"
#endif
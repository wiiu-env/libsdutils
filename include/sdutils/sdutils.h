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
};

enum SDUtilsAttachStatus {
    SDUTILS_ATTACH_MOUNTED   = 1,
    SDUTILS_ATTACH_UNMOUNTED = 0,
};

typedef void (*SDAttachHandlerFn)(SDUtilsAttachStatus status);

/**
 * Initializes the SDUtils library. This must be call before any other function can be called
 * @return SDUTILS_RESULT_SUCCESS on success, the functions of this lib can be used
 *         SDUTILS_RESULT_MODULE_NOT_FOUND when the SDHotSwapModule is not loaded
 *         SDUTILS_RESULT_MODULE_MISSING_EXPORT when the SDHotSwapModule does not export the expected functions.
 */
SDUtilsStatus SDUtils_Init();

/**
 * Deinitializes the SDUtils library, must be called before exiting the application
 * @return SDUTILS_RESULT_SUCCESS on success
 */
SDUtilsStatus SDUtils_DeInit();

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
 *         SDUTILS_RESULT_LIB_UNINITIALIZED if the lib was not initalized properly
 */
SDUtilsStatus SDUtils_AddAttachHandler(SDAttachHandlerFn fn);

/**
 * Removed a previously registered callback
 *
 * @param fn
 * @return SDUTILS_RESULT_SUCCESS on success
 *         SDUTILS_RESULT_NOT_FOUND when the given callback was not registered.
 *         SDUTILS_RESULT_LIB_UNINITIALIZED if the lib was not initalized properly
 */
SDUtilsStatus SDUtils_RemoveAttachHandler(SDAttachHandlerFn fn);

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
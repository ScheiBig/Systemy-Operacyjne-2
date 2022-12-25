/**************************************************************************************
 *                               = == === TODO === == =
 **************************************************************************************
 *
 * - creating named event
 * - conditionally unlinking/closing
 * - waiting for event (trying/timed waiting? dunno if possible on linux)
 * - signaling event
 *
 * On Linux, events should be implemented using eventfd
 * => man eventfd
 * => https://stackoverflow.com/questions/33098419/linux-posix-equivalent-for-win32s-createevent-setevent-waitforsingleobject#:~:text=Check%20also%20eventfd,signaling%2Dwith%2Deventfd
 *
 *************************************************************************************/
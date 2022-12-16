/**************************************************************************************
 *                               = == === TODO === == =
 **************************************************************************************
 *
 * - creating named and unnamed mutex - subclasses
 * - shared behaviour - superclass:
 *   - locking for mutex
 *   - releasing mutex
 *   - try_locking -|\|- - on Win32 constant wait for ~500ms
 *   - timed_locking -|\|- - miliseconds
 *   - conditionally unlinking/closing
 *
 * On Linux, mutex should be implemented using pthread_mutex stored in shared memory
 * => man pthread_mutexattr_getpshared
 * => https://stackoverflow.com/questions/9389730/is-it-possible-to-use-mutex-in-multiprocessing-case-on-linux-unix
 *
 *************************************************************************************/

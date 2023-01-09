/**
  * \file syscalls_stm32.c
  *
  * Implementation of newlib syscall.
  *
  */

#include "ch32v30x.h"
#if defined (  __GNUC__  ) /* GCC CS3 */
  #include <sys/stat.h>
#endif
#include <errno.h>
#undef errno
extern int errno;

// Helper macro to mark unused parameters and prevent compiler warnings.
// Appends _UNUSED to the variable name to prevent accidentally using them.
#ifdef UNUSED
  #undef UNUSED
#endif
#ifdef __GNUC__
  #define UNUSED(x) x ## _UNUSED __attribute__((__unused__))
#else
  #define UNUSED(x) x ## _UNUSED
#endif

void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}


__attribute__((weak))
int _close(UNUSED(int file))
{
  return -1;
}

__attribute__((weak))
int _fstat(UNUSED(int file), struct stat *st)
{
  st->st_mode = S_IFCHR ;
  return 0;
}

__attribute__((weak))
int _isatty(UNUSED(int file))
{
  return 1;
}

__attribute__((weak))
int _lseek(UNUSED(int file), UNUSED(int ptr), UNUSED(int dir))
{
  return 0;
}

__attribute__((weak))
int _read(UNUSED(int file), UNUSED(char *ptr), UNUSED(int len))
{
  return 0;
}

/* Moved to Print.cpp to support Print::printf()
__attribute__((weak))
int _write(UNUSED(int file), char *ptr, int len)
{
}
*/

__attribute__((weak))
void _exit(UNUSED(int status))
{
  for (; ;) ;
}

__attribute__((weak))
int _kill(UNUSED(int pid), UNUSED(int sig))
{
  errno = EINVAL;
  return -1;
}

__attribute__((weak))
int _getpid(void)
{
  return 1;
}

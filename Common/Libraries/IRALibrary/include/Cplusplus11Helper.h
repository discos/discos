#ifndef CPLUSPLUS_HELPER_H
#define CPLUSPLUS_HELPER_H

#if (__cplusplus > 199711L )
#define C11_STRINGIFY(X) #X
#define C11_IGNORE_WARNING_PUSH _Pragma("GCC diagnostic push")
#define C11_IGNORE_WARNING_POP _Pragma("GCC diagnostic pop")
#define C11_IGNORE_WARNING(X) _Pragma(C11_STRINGIFY(GCC diagnostic ignored X))
#endif

#endif
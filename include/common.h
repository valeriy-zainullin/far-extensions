#ifndef COMMON_H
#define COMMON_H

#ifndef TARGET
#error "TARGET" macro is not defined!
#endif

#if TARGET == X86
typedef unsigned int MemoryAddress;
#elif TARGET == X86-64
typedef unsigned long long MemoryAddress;
#else
#error Target is not supported!
#endif

#endif
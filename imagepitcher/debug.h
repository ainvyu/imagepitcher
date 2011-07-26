#pragma once

// file debug.h

#ifdef _DEBUG
void _trace(const TCHAR* fmt, ...);
void _trace_string(const tstring& fmt, ...);

#define ASSERT(x)       { if(!(x)) _asm{ int 0x03 } }
#define VERIFY(x)       { if(!(x)) _asm{ int 0x03 } }
#else
#define ASSERT(x)
#define VERIFY(x)       x
#endif

#ifdef _DEBUG
#define TRACE           _trace
#else
inline void _trace(LPCTSTR fmt, ...) {}
#define TRACE           1 ? (void)0 : _trace
#endif
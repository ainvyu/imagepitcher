#include "stdafx.h"
#include "Debug.h"

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#ifdef _DEBUG
void _trace(const TCHAR* fmt, ...)
{
	TCHAR out[1024];
	va_list body;

	va_start(body, fmt);
	_vstprintf_s(out, fmt, body);
	va_end(body);

	OutputDebugString(out);
}

void _trace_string(const tstring& fmt, ...)
{
	_trace(fmt.c_str());
}

#endif
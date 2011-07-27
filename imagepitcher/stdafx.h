// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#define _WTL_NO_CSTRING

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlddx.h>
#include <atlmisc.h>
#include <atlcrack.h>

#define END_MSG_MAP_EX	END_MSG_MAP

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

#include "pugixml-1.0/src/pugiconfig.hpp"
#include "pugixml-1.0/src/pugixml.hpp"

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_iterators.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"

#ifndef UNICODE
#define tstring std::string
#define tstringstream std::stringstream
#define tpath   boost::filesystem::path
#else
#define tstring std::wstring
#define tstringstream std::wstringstream
#define tpath   boost::filesystem::wpath
#endif //_UNICODE

#include "macro.h"
#include "debug.h"
#include "util.h"
#include "stringutil.h"
#include "stringencode.h"
#include "appdatafile.h"
#include "registermenu.h"
#include "clipboard.h"
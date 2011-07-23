#include "StdAfx.h"

#include "Util.h"

#include "StringUtil.h"

#include <iterator>
#include <shlobj.h>

using namespace std;

void CUtil::DisplayCOMError(_com_error e)
{
  tstring strError;

  _bstr_t bstrSource(e.Source());
  _bstr_t bstrDescription(e.Description());

  CStringUtil::Format(_T("\a Code : %08lx\n\a Code meaning : %s\n  \
                         \a Source : %s\n\a Description : %s\n"), 
                         e.Error(),
                         e.ErrorMessage(),
                         (LPCTSTR) bstrSource,
                         (LPCTSTR) bstrDescription);

  MessageBox(NULL, strError.c_str(), _T(""), MB_ICONERROR);
}

tstring CUtil::GetLastErrorMessage()
{
#ifdef _UNICODE
  return CUtil::GetLastErrorMessageW();
#else
  return CUtil::GetLastErrorMessageA();
#endif // _UNICODE
}

string CUtil::GetLastErrorMessageA()
{
  DWORD dwError = ::GetLastError();
  LPVOID lpMsgBuf;

  FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS, 
    NULL, 
    dwError, 
    0, 
    (LPSTR)&lpMsgBuf, 
    0, 
    NULL );

  string strLastErrMsg(static_cast<LPCSTR>(lpMsgBuf));
  LocalFree(lpMsgBuf);

  return strLastErrMsg;
}

wstring CUtil::GetLastErrorMessageW()
{
  DWORD error = ::GetLastError();
  LPVOID lpMsgBuf;

  FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS, 
    NULL, 
    error, 
    0, 
    (LPWSTR)&lpMsgBuf, 
    0, 
    NULL);

  wstring strLastErrMsg(static_cast<LPCWSTR>(lpMsgBuf));
  LocalFree(lpMsgBuf);

  return strLastErrMsg;
}

tstring CUtil::GetIEPath(void)
{
  HKEY hKey;
  LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
    _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE"), 
    NULL, KEY_READ, &hKey);

  TCHAR strVal[_MAX_PATH] = {0};

  if (lRet == ERROR_SUCCESS)
  {
    long lSize;

    RegQueryValue(hKey, NULL, strVal, &lSize);
  }
  else
  {
    LPVOID lpMsgBuf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
      NULL, lRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,	0, NULL);

    tstring strLastErrMsg(static_cast<LPCTSTR>(lpMsgBuf));
    MessageBox(NULL, strLastErrMsg.c_str(), 
      _T("웹브라우저의 경로를 얻어오는데 실패했습니다."), MB_OK | MB_ICONERROR);
    LocalFree(lpMsgBuf);
  }

  RegCloseKey(hKey);
  return tstring(strVal);
}

bool CUtil::IsVistaOrHigher(void)
{
  OSVERSIONINFO osvi;
  bool bIsWindowsVistaOrHigher;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  GetVersionEx(&osvi);

  bIsWindowsVistaOrHigher = (osvi.dwMajorVersion >= 6);

  return bIsWindowsVistaOrHigher;
}

/*
Usage GetProcessElevation:

TOKEN_ELEVATION_TYPE t;
BOOL bAdmin = FALSE;
char szUser[0xFF] = {0};
DWORD dwUser = _countof(szUser);
GetUserName(szUser, &dwUser);

if ( GetProcessElevation(&t, &bAdmin) )
{
if ( bAdmin )
printf("%s is admin, ", szUser);
else
printf("%s is not admin, ", szUser);

switch (t)
{
case TokenElevationTypeDefault:
printf("기본 사용자이거나, UAC 기능이 OFF 된 상태임.\n");
break;
case TokenElevationTypeFull:
printf("권한상승되었음\n");
break;
case TokenElevationTypeLimited:
printf("필터된 토큰에 의해 제한된 권한으로 수행되었음.\n");
break;
default:
break;
}
}

return 0;
*/
bool CUtil::GetProcessElevation(
  TOKEN_ELEVATION_TYPE* pElevationType, 
  BOOL* pIsAdmin
  )
{
  HANDLE hToken = NULL;
  BOOL bResult = FALSE;
  DWORD dwSize = 0;

  if (!IsVistaOrHigher())
    return FALSE;

  // 현재 프로세스의 토큰을 얻는다.
  if ( !OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) )
    return FALSE;

  // 권한상승 형태에 대한 정보를 얻는다.
  if ( GetTokenInformation(hToken, TokenElevationType, pElevationType,
    sizeof(TOKEN_ELEVATION_TYPE), &dwSize) )
  {
    BYTE adminSID[SECURITY_MAX_SID_SIZE];
    dwSize = sizeof(adminSID);

    // 관리자 그룹의 SID 값을 생성한다.
    CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);

    if ( *pElevationType == TokenElevationTypeLimited )
    {
      HANDLE hUnfilteredToken = NULL;

      // 연결된 토큰의 핸들을 얻는다.
      GetTokenInformation(hToken, TokenLinkedToken, (void *)&hUnfilteredToken, 
        sizeof(HANDLE), &dwSize);

      // 원래의 토큰이 관리자의 SID를 포함하고 있는지 여부를 확인한다.
      if ( CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin) )
        bResult = TRUE;

      CloseHandle(hUnfilteredToken);
    }
    else
    {
      *pIsAdmin = IsUserAnAdmin();
      bResult = TRUE;
    }
  }

  CloseHandle(hToken);
  return bResult != 0;
}

/*
GetProcessElevation 간소화 버젼.
if (SUCCEEDED(IsElevated(&bResult)))
{
if (bResult == TRUE)
something...
else
something...
}
*/
HRESULT CUtil::IsElevated(BOOL* pElevated)
{
  HRESULT hResult = E_FAIL;
  HANDLE hToken = NULL;

  if (!IsVistaOrHigher())
    return hResult;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    return hResult;

  TOKEN_ELEVATION te = { 0 };
  DWORD dwReturnLength = 0;
  const int TokenElevation = 20;

  if (GetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS)TokenElevation, &te, 
    sizeof(te), &dwReturnLength))
  {
    hResult = te.TokenIsElevated ? S_OK : S_FALSE;

    if (pElevated)
    {
      *pElevated = (te.TokenIsElevated != 0);
    }
  }

  CloseHandle(hToken);
  return hResult;
}

bool CUtil::ShellExecWithVerb( 
  HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, 
  LPCTSTR lpParameters, LPCTSTR lpDirectory )
{
  SHELLEXECUTEINFO executeInfo;
  ZeroMemory(&executeInfo, sizeof(executeInfo));

  executeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  executeInfo.fMask = 0;
  executeInfo.hwnd = hWnd;
  executeInfo.lpVerb = lpVerb;
  executeInfo.lpFile = lpPath;
  executeInfo.lpParameters = lpParameters;
  executeInfo.lpDirectory = lpDirectory;
  executeInfo.nShow = SW_NORMAL;

  return ShellExecuteEx(&executeInfo) != 0;
}


bool CUtil::ShellExecWithElevation(
  HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory)
{
  return ShellExecWithVerb(
    hWnd, _T("runas"), lpPath, 
    lpParameters, lpDirectory);
}


bool CUtil::OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl)
{
  TCHAR lpBuffer[MAX_PATH + 1];

  if (!SHGetSpecialFolderPath(hWnd, lpBuffer, CSIDL_PROGRAM_FILES, 0))
    return FALSE;

  _tcscat_s(lpBuffer, _tcslen(lpBuffer), 
    _T("\\Internet Explorer\\iexplore.exe"));
  return ShellExecWithElevation(hWnd, lpBuffer, lpUrl, _T(""));
}

bool CUtil::GetFileTime( const tstring& strPathName, FILETIME& ft )
{
  // open file
  HANDLE hFile = CreateFile(strPathName.c_str(), GENERIC_READ,
    FILE_SHARE_READ, NULL,	OPEN_EXISTING, 0, NULL);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    ZeroMemory(&ft, sizeof(FILETIME));
    ::GetFileTime(hFile, NULL, NULL, &ft);

    CloseHandle(hFile);

    return true;
  }

  return false;
}

tstring CUtil::GetDesktopFolderPath()
{
  TCHAR szPath[MAX_PATH] = { 0, };
  if(!::SHGetSpecialFolderPath(0, szPath, CSIDL_DESKTOP, FALSE)) {
    return _T("");
  }

  return szPath;
}

int CUtil::GetIconIndex(LPCTSTR lpszPath, BOOL bIsDir, BOOL bSelected)
{
  static int _nDirIconIdx = -1;
  SHFILEINFO sfi;
  memset(&sfi, 0, sizeof(sfi));

  if (bIsDir)
  {
    SHGetFileInfo(lpszPath, 
      FILE_ATTRIBUTE_DIRECTORY, 
      &sfi, 
      sizeof(sfi), 
      SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | 
      (bSelected ? SHGFI_OPENICON : 0)
      );

    _nDirIconIdx = sfi.iIcon;

    return _nDirIconIdx;
  }
  else
  {
    SHGetFileInfo (lpszPath, 
      FILE_ATTRIBUTE_NORMAL, 
      &sfi, 
      sizeof(sfi), 
      SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | 
      (bSelected ? SHGFI_OPENICON : 0)
      );

    return sfi.iIcon;
  }

  return -1;
}

BOOL CUtil::InitSystemImageLists(HWND hWnd, BOOL bIsTree)
{	
  SHFILEINFO sfi;

  HIMAGELIST himlSmall = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, 
    &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
  HIMAGELIST himlLarge = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, 
    &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

  if (himlSmall && himlLarge)
  {
    if (bIsTree)
    {
      ::SendMessage(hWnd, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, 
        (LPARAM)himlSmall);
    }
    else
    {
      ::SendMessage(hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, 
        (LPARAM)himlSmall);
      ::SendMessage(hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, 
        (LPARAM)himlLarge);
    }
    return TRUE;
  }

  return FALSE;
} 

//레지스트리에 문자열을 기록
BOOL CUtil::RegSetString(
  HKEY hKeyRoot, tstring strSubKey, tstring strItem, tstring strValue)
{
  HKEY hKey;
  DWORD dwType=REG_SZ, dwSize=16000;	// 98 때문

  if (RegOpenKeyEx(hKeyRoot, strSubKey.c_str(), 0, 
    KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
  {
    // 2000에서 오류로 dwSize 대신 문자열 길이로 변경
    RegSetValueEx(hKey, strItem.c_str(), 0, dwType, 
      (unsigned char *)strValue.c_str(), 
      (strValue.length() + 1) * sizeof(TCHAR));
    RegCloseKey(hKey);
    return TRUE;
  }
  else
  {
    DWORD disp = REG_CREATED_NEW_KEY;
    if (RegCreateKeyEx(hKeyRoot, strSubKey.c_str(), NULL, NULL, 
      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
      &hKey, &disp) == ERROR_SUCCESS )
    {
      // 2000에서 오류로 dwSize 대신 문자열 길이로 변경
      RegSetValueEx(hKey, strItem.c_str(), 0, dwType, (unsigned char *)strValue.c_str(), (strValue.length() + 1) * sizeof(TCHAR));
      RegCloseKey(hKey);
      return TRUE;
    }
  }
  return FALSE;
}

//레지스트리에 기록된 문자열을 얻어옮
tstring CUtil::RegGetString(
  HKEY hKeyRoot, tstring strSubKey, tstring strItem, tstring strDefault)
{
  CHAR szData[16001];	// 98 때문 
  memset(szData, 0, sizeof(szData));
  HKEY hKey;
  DWORD dwType=REG_SZ, dwSize=16000;

  if(RegOpenKeyEx(hKeyRoot, strSubKey.c_str(), 0, 
    KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
  {
    if ( ERROR_SUCCESS != RegQueryValueEx(hKey, strItem.c_str(), 0, 
      &dwType, (unsigned char *)szData, &dwSize) )
    {
      //AfxMessageBox( _T("RegQueryValueEx Error!") );
      return strDefault;
    }
    RegCloseKey(hKey);

    // wide 로 변경 필요 for unicoode
    return (LPCTSTR)(LPCSTR)szData;
  }

  return strDefault;
}

const int MAX_KEY_LENGTH = 256;
DWORD CUtil::RegDeleteSubKey(HKEY hStartKey, LPCTSTR pKeyName )
{
  DWORD   dwRtn, dwSubKeyLength;
  LPTSTR  pSubKey = NULL;
  TCHAR   szSubKey[MAX_KEY_LENGTH]; // (256) this should be dynamic.
  HKEY    hKey;

  // Do not allow NULL or empty key name
  if ( pKeyName &&  lstrlen(pKeyName))
  {
    if( (dwRtn = RegOpenKeyEx(hStartKey,pKeyName,
      0, KEY_ENUMERATE_SUB_KEYS | DELETE, &hKey )) == ERROR_SUCCESS)
    {
      while (dwRtn == ERROR_SUCCESS )
      {
        dwSubKeyLength = MAX_KEY_LENGTH;
        dwRtn = RegEnumKeyEx(hKey,
          0,       // always index zero
          szSubKey,
          &dwSubKeyLength,
          NULL,
          NULL,
          NULL,
          NULL);

        if(dwRtn == ERROR_NO_MORE_ITEMS)
        {
          dwRtn = RegDeleteKey(hStartKey, pKeyName);
          break;
        }
        else if(dwRtn == ERROR_SUCCESS)
        {
          dwRtn = RegDeleteSubKey(hKey, szSubKey);
        }
      }
      RegCloseKey(hKey);
      // Do not save return code because error
      // has already occurred
    }
  }
  else
  {
    dwRtn = ERROR_BADKEY;
  }

  return dwRtn;
}

BOOL CUtil::MakePath(tstring strPath)
{
  list<tstring> lstPath;
  CStringUtil::Tokenize(strPath, lstPath, _T("\\/"));

  if (lstPath.empty()) return TRUE;

  tstring strDir = lstPath.front();

  if (!FindDir(strDir.c_str()))
  {
    if (!CreateDirectory(strDir.c_str(), NULL))
      return 0;
  }

  for(auto it = lstPath.begin(), e = lstPath.end(); it != e; ++it)
  {
    strDir += _T("\\") + (*it);

    if (!CUtil::FindDir(strDir.c_str()))
    {
      if (!CreateDirectory(strDir.c_str(), NULL))
        return 0;
    }
  }

  return TRUE;
}

BOOL CUtil::FindDir(LPCTSTR lpDir)
{
  DWORD ret = GetFileAttributes(lpDir);

  if(ret != -1)
  {
    if(ret & FILE_ATTRIBUTE_DIRECTORY)
    {
      return TRUE;
    }
  }

  return FALSE;
}

// list에 path를 경로로 표시
tstring CUtil::GetMergeReference(list<tstring>& lstToken)
{
  tstring strResult;
  BOOL bRoof = FALSE;

  for (auto it = lstToken.begin(), e = lstToken.end(); it != e; ++it)
  {
    strResult += _T("/") + (*it);
    bRoof = TRUE;
  }

  return strResult;
}

void CUtil::CopyMap(
  const map<tstring, tstring>& map1, map<tstring, tstring>& map2)
{
  copy(map1.begin(), map1.end(), inserter(map2, map2.begin()));
}

void CUtil::DebugString(TCHAR *szMsg, ...)
{
  HANDLE hCon;
  DWORD dwWrite;
  TCHAR szBuffer[1024] = { 0 };

  va_list args;
  va_start(args, szMsg);
  _vsntprintf_s(szBuffer, sizeof(szBuffer), szMsg, args);
  va_end(args);

  AllocConsole();
  hCon = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteConsole(hCon, szBuffer, (int)_tcslen(szBuffer), &dwWrite, NULL);
}

// void CUtil::NormalizePath(tstring& strPath)
// {
// 	if(strPath.empty()) 
// 		return;
// 
// 	CStringUtil::Replace(strPath, _T("\\"), _T("/")); // / 로 통일..
// 
// 	// 마지막 / 를 없앤다..
// 	const int nLen = strPath.length();
// 
// 	if(strPath.at(nLen-1) == _T('/'))
// 	{
// 		strPath = strPath.Left(nLen-1);
// 	}
// 
// 	// ../ 상위로 이동
// 	int nFind  = 0;
// 
// 	while((nFind = strPath.find(_T("../"))) != -1)
// 	{
// 		int i = 0;
// 
// 		// 반대로 / 를 찾는다..
// 		for(i=nFind-2 /* /../ */; i>=0; i--)
// 		{
// 			const TCHAR c = strPath.at(i);
// 			if(c != '/') continue;
// 
// 			strPath = strPath.Left(i+1) + strPath.Mid(nFind + 3/* ../ */);
// 			break;
// 		}		
// 
// 		// 에러?? C:/../global.js 같은 경우?
// 
// 		if(i <= -1) break;
// 	}
// 
// 	// ./ 없앤다...
// 	
// 	CStringUtil::Replace(strPath, (_T("./"), _T(""));
// 
// 	// 맨앞에 \\211. 이런 경우 IE 가 열지 못하기 때문에 원상 복귀...
// 	if(strPath.length() > 2)
// 	{
// 		if(strPath.at(0) == _T('/') && strPath.at(1) == _T('/'))
// 		{
// 			strPath.at(0) = '\\';
// 			strPath.at(1) = '\\';
// 		}
// 	}
// }

list<tstring> CUtil::GetFiles( LPCTSTR path )
{
  list<tstring> retval;
  WIN32_FIND_DATA data;

  tstring pathQuery = path;
  pathQuery += _T("\\*.*");

  HANDLE hFindFile = FindFirstFile( pathQuery.c_str(), &data );

  if (INVALID_HANDLE_VALUE != hFindFile)
  {
    BOOL bSuccess = TRUE;
    while (bSuccess)
    {
      retval.push_back(data.cFileName);
      bSuccess = FindNextFile(hFindFile, &data);
    }

    FindClose(hFindFile );
  }

  return retval;
}

BOOL CUtil::RecursiveDeleteFile( LPCTSTR path )
{
  DWORD attr = GetFileAttributes( path );

  if ( FILE_ATTRIBUTE_DIRECTORY & attr )
  {
    //case of directory

    //find sub files
    list<tstring> fileInfoList = GetFiles( path );

    //delete file
    for (auto it = fileInfoList.cbegin(), e = fileInfoList.cend(); it != e; 
        ++it )
    {
      const tstring& strPath = (*it);
      if (   _T(".")  != strPath 
        && _T("..") != strPath )
      {
        tpath pathFile(path);
        pathFile /= strPath;
        CUtil::RecursiveDeleteFile( pathFile.wstring().c_str() );
      }
    }

    return RemoveDirectory( path );
  }
  else
  {
    //case of not directory
    return DeleteFile( path );
  }
}

bool CUtil::RenameFile( 
  tstring const& strOldFilePathName, tstring const& strNewFilePathName )
{
  if (MoveFile(strOldFilePathName.c_str(), strNewFilePathName.c_str()) == 0)
    return false;

  return true;
}

void CUtil::SplitPath( 
  tstring const& strPath, tstring& strDrive, 
  tstring& strDir, tstring& strFileName, tstring& strExt )
{
  TCHAR drive[_MAX_DRIVE];
  TCHAR dir[_MAX_DIR];
  TCHAR fname[_MAX_FNAME];
  TCHAR ext[_MAX_EXT];

  _tsplitpath_s(strPath.c_str(), 
    drive, _MAX_DRIVE,
    dir,   _MAX_DIR,
    fname, _MAX_FNAME,
    ext,   _MAX_EXT);

  strDrive = drive;
  strDir = dir;
  strFileName = fname;
  strExt = ext;
}

tstring CUtil::ChangeDirectoryToModulePath()
{
  TCHAR szOldFilePath[_MAX_PATH] = {0};
  GetCurrentDirectory(_MAX_PATH, szOldFilePath);

  // make current module path
  TCHAR szFilePath[_MAX_PATH] = {0};
  GetModuleFileName(NULL, szFilePath, _MAX_PATH);

  tstring strDrive, strDir, strFileName, strExt;
  CUtil::SplitPath(szFilePath, strDrive, strDir, strFileName, strExt);
  tstring strCurDirPath = strDrive + strDir;

  SetCurrentDirectory(strCurDirPath.c_str());

  return szOldFilePath;
}

tstring CUtil::GetTempFileNameSimple(LPCTSTR szExt)
{
  tstring retval;

  const long BUF_SIZE = 512;
  TCHAR buf[BUF_SIZE];
  TCHAR lpPathBuf[BUF_SIZE];

  // Get the temp path
  DWORD dwRetVal = GetTempPath( BUF_SIZE, lpPathBuf );

  if( dwRetVal > BUF_SIZE || 0 == dwRetVal )
  {
    throw runtime_error(CUtil::GetLastErrorMessageA());
  }

  dwRetVal = GetTempFileName( lpPathBuf, szExt, 0, buf );

  if( 0 == dwRetVal )
  {
    throw runtime_error(CUtil::GetLastErrorMessageA());
  }

  retval = buf;

  tpath path(retval);
  path.replace_extension(szExt);

  retval = path.wstring();

  return retval;
}

tstring CUtil::GetSizeWithUnitString(long long nFileSize, int nPrecision)
{
  long double dConvertSize = (long double)nFileSize;

  if (nFileSize >= 1024)
  {
    while (dConvertSize >= 1024)
    {
      dConvertSize = dConvertSize / 1024.0;
    }
  }

  TCHAR strFormat[8] = {0};

  if (nPrecision == 0)
  {
    return CStringUtil::Format(_T("%d"), (int)dConvertSize) + 
      GetSizeUnitString(nFileSize);
  }
  else
  {
    _stprintf(strFormat, _T("%%.%d%s"), nPrecision, _T("f"));
    return CStringUtil::Format(strFormat, dConvertSize) + 
      GetSizeUnitString(nFileSize);
  }
}

// tstring CUtil::GetSizeWithUnitString(long long nFileSize)
// {
// 	tstring strSizeUnit;
// 	long long nConvertFileSize = nFileSize;
// 	if (nFileSize >= 1024 * 1024 * 1024 ) // GB
// 	{
// 		nConvertFileSize /= 1024 * 1024 * 1024;
// 		strSizeUnit = _T("GB");
// 	}
// 	else if (nFileSize >= 1024 * 1024) // MB
// 	{
// 		nConvertFileSize /= 1024 * 1024;
// 		strSizeUnit = _T("MB");
// 	}
// 	else if (nFileSize >= 1024) // KB
// 	{
// 		nConvertFileSize /= 1024;
// 		strSizeUnit = _T("KB");
// 	}
// 	else 
// 	{
// 		strSizeUnit = _T("B");
// 	}
// 
// 	return CStringUtil::Int64ToStr(nConvertFileSize) + strSizeUnit;
// }

tstring CUtil::GetSizeUnitString(long long nFileSize)
{
  tstring strSizeUnit;
  if (nFileSize >= 1024 * 1024 * 1024 ) // GB
  {
    strSizeUnit = _T("GB");
  }
  else if (nFileSize >= 1024 * 1024) // MB
  {
    strSizeUnit = _T("MB");
  }
  else if (nFileSize >= 1024) // KB
  {
    strSizeUnit = _T("KB");
  }
  else 
  {
    strSizeUnit = _T("B");
  }

  return strSizeUnit;
}

tstring CUtil::GetModulePathName(void)
{
  TCHAR szPath[MAX_PATH];
  ::GetModuleFileName(::GetModuleHandle(NULL), szPath, MAX_PATH);

  return szPath;
}

tstring CUtil::GetModulePath(void)
{
  TCHAR szPath[MAX_PATH];
  ::GetModuleFileName(::GetModuleHandle(NULL), szPath, MAX_PATH);

  TCHAR drive[_MAX_DRIVE];
  TCHAR dir[_MAX_DIR];
  TCHAR fname[_MAX_FNAME];
  TCHAR ext[_MAX_EXT];

  _tsplitpath_s(szPath, drive, dir, fname, ext);

  TCHAR szModulePath[MAX_PATH];
  _stprintf(szModulePath, _T("%s%s"), drive, dir);

  return tstring(szModulePath);
}

DWORD RegDeleteSubKey(HKEY hStartKey, LPCTSTR pKeyName )
{
  DWORD   dwRtn, dwSubKeyLength;
  LPTSTR  pSubKey = NULL;
  TCHAR   szSubKey[MAX_KEY_LENGTH]; // (256) this should be dynamic.
  HKEY    hKey;

  // Do not allow NULL or empty key name
  if ( pKeyName &&  lstrlen(pKeyName))
  {
    if( (dwRtn = RegOpenKeyEx(hStartKey,pKeyName,
      0, KEY_ENUMERATE_SUB_KEYS | DELETE, &hKey )) == ERROR_SUCCESS)
    {
      while (dwRtn == ERROR_SUCCESS )
      {
        dwSubKeyLength = MAX_KEY_LENGTH;
        dwRtn = RegEnumKeyEx(hKey,
          0,       // always index zero
          szSubKey,
          &dwSubKeyLength,
          NULL,
          NULL,
          NULL,
          NULL);

        if(dwRtn == ERROR_NO_MORE_ITEMS)
        {
          dwRtn = RegDeleteKey(hStartKey, pKeyName);
          break;
        }
        else if(dwRtn == ERROR_SUCCESS)
        {
          dwRtn = RegDeleteSubKey(hKey, szSubKey);
        }
      }
      RegCloseKey(hKey);
      // Do not save return code because error
      // has already occurred
    }
  }
  else
  {
    dwRtn = ERROR_BADKEY;
  }

  return dwRtn;
}

unsigned long long CUtil::GetTickCount( void )
{
#ifdef _WIN32_WINDOWS
  return ::GetTickCount64();
#else
  return time(NULL);
#endif
}

LPITEMIDLIST ConvertPathToLpItemIdList(const TCHAR* pszPath)
{
  LPITEMIDLIST  pidl = NULL;
  LPSHELLFOLDER pDesktopFolder;

  if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
  {
    OLECHAR       olePath[MAX_PATH];
    ULONG         chEaten;
    ULONG         dwAttributes;
    HRESULT       hr;

    _tcscpy_s(olePath, pszPath);
    hr = pDesktopFolder->ParseDisplayName(NULL, NULL, olePath, 
      &chEaten, &pidl, &dwAttributes);
    pDesktopFolder->Release();
  }

  return pidl;
}

int CALLBACK BrowseCallbackProc(
  HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  switch(uMsg)
  {
  case BFFM_INITIALIZED: 
    {
      SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
      break;
    }

  case BFFM_SELCHANGED: 
    {
      TCHAR szText[MAX_PATH] = {0};
      SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), szText);
      SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, 
        reinterpret_cast<LPARAM>(szText));
      break;
    }
  }

  return 0;
}

tstring CUtil::SelectDirectory(
  HWND parentWnd, tstring title, tstring displayPath)
{
  TCHAR szWorkPath[MAX_PATH] = {0};

  //ConvertPathToLpItemIdList(_T("E:\animations\new"));//szRootPath);
  LPITEMIDLIST pidlBrowse;
  BROWSEINFO bi;
  bi.hwndOwner      = parentWnd; // 부모 윈도우의 핸들
  bi.pidlRoot       = NULL;
  bi.pszDisplayName = szWorkPath;
  bi.lpszTitle      = title.c_str();
  bi.ulFlags        = BIF_RETURNONLYFSDIRS | 
                      BIF_BROWSEINCLUDEFILES | BIF_USENEWUI;
  bi.lpfn           = BrowseCallbackProc; // 기본값 선택을 위한 콜백
  bi.lParam         = (LPARAM)displayPath.c_str();

  pidlBrowse = SHBrowseForFolder(&bi);
  if (pidlBrowse != NULL)
  {
    if (!SHGetPathFromIDList(pidlBrowse, szWorkPath)) // 패스를 얻어온다. 
    {
      return _T("");
    }
  }

  return szWorkPath;
}

/*
경로명을 가지고 파일의 크기를 구하려고 할때 제일 먼저 생각나는 API 는 GetFileSize() 지만
문제는 이 API 를 사용하기 위해서는 CreateFile 로 파일을 열고 닫아야 하는데, 
이 때문에 속도가 매우 느리다.
더 빠르고 좋은 (권한 문제등 때문에) 방법은 FindFirstFile() 을 사용하는 것이다.
*/
INT64 CUtil::GetFileSize( LPCTSTR szPathName )
{
  INT64   fileSize = -1;
  WIN32_FIND_DATA findData;
  HANDLE h = FindFirstFile(szPathName, &findData);
  if(h != INVALID_HANDLE_VALUE)
  {
    fileSize = (((INT64)findData.nFileSizeHigh)<<32) + findData.nFileSizeLow;
    FindClose(h);
  }
  return fileSize;
}

std::string CUtil::itob(int n) {
  char arr[32 + 1] = {'0'};
  int cnt = 32;

  while (cnt--) { 
    arr[cnt] = '0' + static_cast<char>(n & 0x1);
    n >>= 1;
  }

  return string(arr);
}

// std::string CUtil::itob(unsigned int n) {
//     char arr[32 + 1] = {'0'};
//     int cnt = 32;
// 
//     while (cnt--) { 
//         arr[cnt] = '0' + static_cast<char>(n & 0x1);
//         n >>= 1;
//     }
// 
//     return string(arr);
// }

void CUtil::itob( int n, char arr[], int length )
{
  int cnt = length;

  while (cnt--) {
    arr[cnt] = '0' + static_cast<char>(n & 0x1);
    n >>= 1;
  }
}

int CUtil::btoi( char* str )
{
  int n = 0;
  int cnt = 0;

  while (str[cnt]) {
    n = (n << 1) | (str[cnt++] - '0');
  }

  return n;
}

// unsigned int CUtil::btoi( char* str )
// {
//     unsigned n = 0;
//     int cnt = 0;
// 
//     while (str[cnt]) {
//         n = (n << 1) | (str[cnt++] - '0');
//     }
// 
//     return n;
// }


int CUtil::btoi( char arr[], int length )
{
  int n = 0;
  int cnt = 0;

  while (length--) {
    n = (n << 1) | (arr[cnt++] - '0');
  }

  return n;
}

ITEMIDLIST* CUtil::IDListFromPath(LPCTSTR pathName)
{
  LPSHELLFOLDER   pDesktopFolder;
  HRESULT         hr = SHGetDesktopFolder(&pDesktopFolder);
  ITEMIDLIST*     pidl=NULL;

  if (FAILED(hr)) {
    assert(0); 
    return NULL;
  }

  OLECHAR olePath [MAX_PATH];

#ifdef _UNICODE
  lstrcpy(olePath, pathName);
#else
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszPath, -1, pathName, MAX_PATH);
#endif

  ULONG chEaten;
  ULONG dwAttributes;
  hr = pDesktopFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidl, &dwAttributes);

  pDesktopFolder->Release();
  return pidl;
}

// 폴더 열때 해당 경로의 파일이 선택되게하는 함수
// Usage: 
// OpenFolderAndSelectItems(_T("C:\\temp\\test.txt"));
//
// SHOpenFolderAndSelectItems() 는 wine 에서 not implemented 에러를 토해내고 죽으니
// wine 사용을 고려하면 피하자.
BOOL CUtil::OpenFolderAndSelectItems(LPCTSTR filePathName)
{
  BOOL ret = TRUE;

  ITEMIDLIST* pidlFile = IDListFromPath(filePathName);

  if (pidlFile == NULL || 
    FAILED(SHOpenFolderAndSelectItems(pidlFile, 0, NULL, 0)))
  {
    assert(0); 
    ret = FALSE;
  }

  if (pidlFile) {
    ::ILFree(pidlFile);
  }

  return ret;
}

// explorer /select,c:\down\setup.exe
BOOL CUtil::OpenFolderAndSelectItemsSimple(LPCTSTR filePathName)
{
  tstringstream cmd;
  cmd << _T("/select,") << filePathName;
  ShellExecute(NULL, _T("open"), _T("explorer"), cmd.str().c_str(), NULL, SW_SHOW);
  return TRUE;
}
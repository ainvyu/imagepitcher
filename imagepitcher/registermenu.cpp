#include "StdAfx.h"

#define _COMPILING_DLL
#include "RegisterMenu.h"
#include <shlobj.h>

#include "Util.h"
#include "StringUtil.h"

bool CRegisterMenu::RegisterMenu(const tstring& strSubKey, const tstring& strValue)
{
  HKEY hKey;
  DWORD strValueSize = strValue.empty() ? 0 : (strValue.length() + 1) * sizeof(TCHAR);

  if (RegOpenKeyEx(HKEY_CLASSES_ROOT, strSubKey.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS )
  {
    if (!strValue.empty())
      RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)strValue.c_str(), strValueSize);

    RegCloseKey(hKey);
  }
  else
  {
    DWORD disp = REG_CREATED_NEW_KEY;
    if (RegCreateKeyEx(HKEY_CLASSES_ROOT, strSubKey.c_str(), 
      NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
      &hKey, &disp) == ERROR_SUCCESS )
    {
      if (!strValue.empty())
        RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)strValue.c_str(), strValueSize);

      RegCloseKey(hKey);
    }
  }

  return true;
}

bool CRegisterMenu::UnRegisterMenu(const tstring& strSubKey)
{
  if (CUtil::RegDeleteSubKey(HKEY_CLASSES_ROOT, strSubKey.c_str()) != ERROR_SUCCESS)
  {
    return false;
  }

  return true;
}

//  register extension key
//  ex) HKEY_CLASSES_ROOT
//        *
//          Shell
//            Watched
//              command - "d:\Development\project\FileMarker\Debug\FileMarker.exe" "-p [watched]" "-r [watched]" "%1"
bool CRegisterMenu::RegisterExtKey(const tstring& strExt, 
                                   const tstring& strMenu, 
                                   const tstring& strMenuDisplay, 
                                   const tstring& strCommand )
{
  LPCTSTR strSubKeys[] = { strExt.c_str(), 
                           _T("shell"), 
                           strMenu.c_str(),
                           _T("command") 
  };

  LPCTSTR strValues[] = { _T(""),
                          _T(""),
                          strMenuDisplay.c_str(),
                          strCommand.c_str() };

  tstring strSubKey;
  tstring strValue;
  for (int i = 0; i < _countof(strSubKeys); ++i)
  {
    strSubKey += strSubKeys[i];
    strSubKey += _T("\\");
    strValue = strValues[i];

    if (!RegisterMenu(strSubKey, strValue))
      return false;
  }

  return true;
}

bool CRegisterMenu::UnRegisterExtKey(const tstring& strExt, const tstring& strMenu)
{
  return UnRegisterMenu(strExt + _T("\\shell\\") + strMenu + _T("\\"));
}

// HKEY_CLASSES_ROOT
//    FileMarker.AVI - AVI - Windows 기본 비디오 파일
//       shell
// 	       Mark - Daum 팟플레이어의 재생목록(&I)에 추가하기
// 		    command - "C:\Program Files (x86)\DAUM\PotPlayer\PotPlayer.exe" "%1" /ADD
bool CRegisterMenu::Register(const tstring& strAppName, tstring strExt, 
                             tstring strMenu, tstring strMenuDisplay, 
                             tstring strCommand, tstring strDesc)
{
  if (strAppName.empty() || strExt.empty() || strDesc.empty() || 
      strMenu.empty() || strMenuDisplay.empty() || strCommand.empty())
    return false;

  if (!RegisterExtKey(strExt, strMenu, strMenuDisplay, strCommand))
    return false;

  // remove '.'
  strExt.erase(std::remove(strExt.begin(), strExt.end(), _T('.')), strExt.end());

  // Uppercase extension
  CStringUtil::MakeUpper(strExt);
  tstring strAppExt = strAppName + _T(".") + strExt;

  LPCTSTR strSubKeys[] = { strAppExt.c_str(), 
                           _T("shell"), 
                           strMenu.c_str(), 
                           _T("command") };

  LPCTSTR strValues[] = { strDesc.c_str(),
                          _T(""),
                          strMenuDisplay.c_str(),
                          strCommand.c_str() };

  tstring strSubKey;
  tstring strValue;
  for (int i = 0; i < _countof(strSubKeys); ++i)
  {
    strSubKey += strSubKeys[i];
    strSubKey += _T("\\");
    strValue = strValues[i];

    if (!RegisterMenu(strSubKey, strValue))
      return false;
  }

//	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);

  return true;
}

bool CRegisterMenu::UnRegister(tstring strAppName, tstring strExt, tstring strMenu)
{
  if (   strAppName.empty()
    || strExt.empty() 
    || strMenu.empty())
    return false;

  if (!UnRegisterExtKey(strExt, strMenu))
  {
    return false;
  }

  // remove '.'
  strExt.erase(std::remove(strExt.begin(), strExt.end(), _T('.')), strExt.end());

  // Uppercase extension
  CStringUtil::MakeUpper(strExt);
  tstring strAppExt = strAppName + _T(".") + strExt;

  return UnRegisterMenu(strAppExt);	
}
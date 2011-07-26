#include "stdafx.h"

#include "clipboard.h"
#include "debug.h"

#ifdef UNICODE
#define CLIPBOARDFMT    CF_UNICODETEXT
#else
#define CLIPBOARDFMT    CF_TEXT
#endif

// Copies the text to the clipboard.
bool CClipboard::CopyTextToClipboard(LPCTSTR szMem, HWND hWndOwner )
{
	ASSERT ( FALSE == IsBadStringPtr(szMem, MAX_PATH) );
	if ( TRUE == IsBadStringPtr(szMem, MAX_PATH) )
	{
		return FALSE;
	}

	bool bRet = TRUE;
	BOOL bOpenedCB = FALSE;
	if ( TRUE == ::OpenClipboard(hWndOwner) )
	{
		bOpenedCB = TRUE;
		if ( TRUE == ::EmptyClipboard() )
		{
			// Do the goofy memory stuff.
			HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, (_tcslen( szMem ) + 1) * sizeof(TCHAR));

			if ( NULL != hGlobal )
			{
				TCHAR* szClipMem = (TCHAR*)GlobalLock(hGlobal);
				ASSERT( NULL != szMem );

				_tcscpy(szClipMem , szMem);

				GlobalUnlock ( hGlobal );
				if ( NULL == ::SetClipboardData(CLIPBOARDFMT, hGlobal) )
				{
					ASSERT(!_T("SetClipboardData failed!!"));
				}
			}
			else
			{
				ASSERT(!_T("Unable to GlobalAlloc memory!!"));
				bRet = FALSE;
			}
		}
		else
		{
			ASSERT(!_T("Unable to empty the clipboard!!"));
			bRet = FALSE;
		}
	}
	else
	{
		ASSERT(!_T("Unable to open the clipboard!!"));
		bRet = FALSE;
	}
	if ( TRUE == bOpenedCB )
	{
		CloseClipboard( );
	}

	return bRet;
}

tstring CClipboard::GetTextFromClipboard(HWND hWnd)
{
	if (!::IsClipboardFormatAvailable(CLIPBOARDFMT))	return _T("");
	if (!::OpenClipboard(hWnd))							return _T("");

	tstring strClipboardText;
	HGLOBAL hGlobal = ::GetClipboardData(CLIPBOARDFMT);
	if (NULL != hGlobal)
	{
		TCHAR* p = (TCHAR *)::GlobalLock(hGlobal);
		if (NULL != p)
		{
			strClipboardText = p;
		}
		else
		{
			ASSERT(!_T("Fail to GlobalLock!!"));
		}

		::GlobalUnlock(hGlobal);
	}
	else
	{
		ASSERT(!_T("Fail to GetClipboardData!!"));
	}

	::CloseClipboard();

	return strClipboardText;
}
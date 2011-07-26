#pragma once

class CClipboard
{
public:
	static bool		CopyTextToClipboard(LPCTSTR szMem, HWND hWndOwner = NULL);
	static tstring	GetTextFromClipboard(HWND hWnd = NULL);
};

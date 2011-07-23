// TextFile.h: interface for the CTextFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <fstream>

#define FILE_ENCODING_ANSI		1
#define FILE_ENCODING_UTF8		2
#define FILE_ENCODING_UTF16		3

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

//#pragma warning(disable: 4251)

struct TextFilePimpl;

class CTextFile
{
public:
	CTextFile();
	virtual ~CTextFile();

	static const char utf8_bom[];
	static const char utf16_bom_little[];

	BOOL Open( LPCTSTR lpszFileName, UINT nOpenFlags);
	void Close();
	
	void  SeekToBegin();
	int	 SeekToTextBegin();
	
	WORD	GetEncoding();
	
	void Flush();
	void Write(const void* lpBuf, UINT nCount);
	void Read(void* lpBuf, UINT nCount);

	void	SetText(LPCTSTR szText, WORD wFileEncodingType);
	tstring GetText();	

	std::streamoff GetLength();

private:
	int		UTF8ToUnicode( LPBYTE src, LPWSTR dst );
	int		UnicodeToUTF8( LPCWSTR src, LPBYTE dst = NULL );

protected:
	TextFilePimpl* m_pImpl;
};
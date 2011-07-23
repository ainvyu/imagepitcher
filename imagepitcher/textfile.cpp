#include "stdafx.h"

#define _COMPILING_DLL
#include "TextFile.h"

using namespace std;

const char CTextFile::utf8_bom[] = {'\xef', '\xbb', '\xbf'};
const char CTextFile::utf16_bom_little[] = {'\xff', '\xfe'};

struct TextFilePimpl
{
	fstream fs;
	WORD    wEncodeType;
};

CTextFile::CTextFile()
: m_pImpl(new TextFilePimpl)
{
	m_pImpl->wEncodeType = 0;
}

CTextFile::~CTextFile()
{
	SAFE_DELETE(m_pImpl);
}

// 이걸 열때는 ios::binary를 왠만하면 넣어주는게 좋다.
BOOL CTextFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags)
{
// BOOL bResult = CFile::Open(lpszFileName, nOpenFlags, pError);
	m_pImpl->fs.open(lpszFileName, nOpenFlags);

	BOOL bResult = m_pImpl->fs.is_open();
	m_pImpl->wEncodeType = NULL;

	if (bResult && !(nOpenFlags & ios_base::out))
	{
		char buf[3] = { 0, };

		m_pImpl->fs.read(buf, 3);
		if (strncmp(buf, utf8_bom, 3) == 0)		
			m_pImpl->wEncodeType = FILE_ENCODING_UTF8;
		else if (strncmp(buf, utf16_bom_little, 2) == 0)
		{
			m_pImpl->fs.seekg(2, ios::beg);
			m_pImpl->wEncodeType = FILE_ENCODING_UTF16;
		}
		else
		{
			m_pImpl->fs.seekg(0, ios::beg);
			m_pImpl->wEncodeType = FILE_ENCODING_ANSI;
		}
	}

	return bResult;
}

void CTextFile::Close()
{
	m_pImpl->fs.close();
}

WORD CTextFile::GetEncoding()
{
	return m_pImpl->wEncodeType;
}

void CTextFile::SeekToBegin()
{
	m_pImpl->fs.seekg(0, ios::beg);
}


int CTextFile::SeekToTextBegin()
{
	int nSeek = 0;

	if (m_pImpl->wEncodeType == FILE_ENCODING_UTF8)		 nSeek = 3;
	else if (m_pImpl->wEncodeType == FILE_ENCODING_UTF16) nSeek = 2;

//	Seek(nSeek, CFile::begin);
	m_pImpl->fs.seekg(nSeek, ios::beg);

	return nSeek;
}

streamoff CTextFile::GetLength()
{
	streamoff nOldPos = m_pImpl->fs.tellg();
	m_pImpl->fs.seekg(0, std::ios::end);
	streamoff nSize = m_pImpl->fs.tellg();
	m_pImpl->fs.seekg(nOldPos);

	return nSize;
}

tstring CTextFile::GetText()
{
	tstring strText;

	long lSize = (long)GetLength();
	lSize -= SeekToTextBegin();

	BYTE* buf = new BYTE[lSize + 2];
	ZeroMemory(buf, lSize + 2);
//	Read(buf, lSize);
	m_pImpl->fs.read((LPSTR)buf, lSize);

	if (m_pImpl->wEncodeType == FILE_ENCODING_UTF8)
	{
		LPWSTR wideChars = new WCHAR[lSize + 1];
		ZeroMemory( wideChars, (lSize + 1) * sizeof(WCHAR) );
		UTF8ToUnicode( buf, wideChars );

		strText = wideChars;

		delete wideChars;
	}
	else if (m_pImpl->wEncodeType == FILE_ENCODING_UTF16)
	{
		strText = (LPCTSTR)buf;
	}
	else
	{
		strText = tstring((LPTSTR)buf);
	}

	delete buf;

	return strText;
}

void CTextFile::SetText(LPCTSTR szText, WORD wFileEncodingType)
{
	if (wFileEncodingType == FILE_ENCODING_UTF8)
	{
		int nLen = UnicodeToUTF8(szText);
		BYTE* pBuff = new BYTE[nLen];
		UnicodeToUTF8(szText, pBuff);
		
//		Write(utf8_bom, 3);
		m_pImpl->fs.write(utf8_bom, 3);
//		Write(pBuff, nLen * sizeof(BYTE));
		m_pImpl->fs.write((LPCSTR)pBuff, nLen * sizeof(BYTE));

		delete []pBuff;
	}
	else if (wFileEncodingType == FILE_ENCODING_UTF16)
	{
//		Write(utf16_bom_little, 2);
		m_pImpl->fs.write((LPCSTR)utf16_bom_little, 2);
//		Write(szText, (UINT)(_tcslen(szText) * sizeof(TCHAR)));
		m_pImpl->fs.write((LPCSTR)szText, (UINT)(_tcslen(szText) * sizeof(TCHAR)));
	}
	else
	{
		int nLen = ::WideCharToMultiByte(CP_ACP, NULL, szText, (int)_tcslen(szText), NULL, NULL, NULL, NULL);
		LPSTR szBuff = new char[nLen];
		::WideCharToMultiByte(CP_ACP, NULL, szText, (int)_tcslen(szText), szBuff, nLen, NULL, NULL);

//		Write(szBuff, nLen);
		m_pImpl->fs.write(szBuff, nLen);

		delete []szBuff;
	}
}

int CTextFile::UTF8ToUnicode( LPBYTE src, LPWSTR dst )
{
	int p = 0;
	int q = 0;
	WCHAR d;

	while( src[p] ) 
	{
		if( (src[p]&0xE0) ==0xE0 ) 
		{
			d = ((src[p]&0x0f)<<12) | ((src[p+1]&0x3F)<<6) | (src[p+2]&0x3F);
			p+=3;
		} 
		else if( (src[p]&0xC0) ==0xC0 ) 
		{
			d = ((src[p]&0x1F)<<6) | (src[p+1]&0x3F);
			p+=2;
		} 
		else 
		{
			d = src[p]&0x7F;
			p++;
		}

		dst[q] = d;
		q++;
	}

	dst[q] = NULL;
	return q;
}

int CTextFile::UnicodeToUTF8( LPCWSTR src, LPBYTE dst /* = NULL*/ )
{
	int p = 0;
	int q = 0;

	if (dst)
	{
		while( src[p] ) 
		{
			if( src[p] < 0x0080 )
			{
				dst[q++] = src[p]&0x7F;
			} 
			else if( src[p] < 0x0800 )
			{
				dst[q++] = 0xC0 | (BYTE)((src[p] & 0x07c0) >> 6);
				dst[q++] = 0x80 | (BYTE)(src[p] & 0x003f);
			} 
			else 
			{
				dst[q++] = 0xE0 | (BYTE)((src[p] & 0xf000) >> 12);
				dst[q++] = 0x80 | (BYTE)((src[p] & 0x0fc0) >> 6);
				dst[q++] = 0x80 | (BYTE)(src[p] & 0x003f);
			}
			p++;
		}
	}
	else
	{
		while(src[p]) 
		{
			if (src[p] < 0x0080)				q++;
			else 
			if (src[p] < 0x0800)				q += 2;
			else 								q += 3;

			p++;
		}
	}

	return q;
}

void CTextFile::Flush()
{
	m_pImpl->fs.flush();
}

void CTextFile::Write( const void* lpBuf, UINT nCount )
{
	m_pImpl->fs.write((LPSTR)lpBuf, nCount);
}

void CTextFile::Read( void* lpBuf, UINT nCount )
{
	m_pImpl->fs.read((LPSTR)lpBuf, nCount);
}
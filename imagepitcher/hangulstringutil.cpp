#include "StdAfx.h"

#include "HangulStringUtil.h"

//초성 
static const wchar_t wcHead[] = {
	L'ㄱ', L'ㄲ', L'ㄴ', L'ㄷ', L'ㄸ', L'ㄹ', L'ㅁ', L'ㅂ', L'ㅃ', L'ㅅ', 
	L'ㅆ', L'ㅇ', L'ㅈ', L'ㅉ', L'ㅊ', L'ㅋ',	L'ㅌ', L'ㅍ', L'ㅎ'
};

//중성
static const wchar_t wcMid[] = {
	L'ㅏ', L'ㅐ', L'ㅑ', L'ㅒ', L'ㅓ', L'ㅔ', L'ㅕ', L'ㅖ', L'ㅗ', L'ㅘ', 
	L'ㅙ', L'ㅚ', L'ㅛ', L'ㅜ', L'ㅝ', L'ㅞ',	L'ㅟ', L'ㅠ', L'ㅡ', L'ㅢ', 
	L'ㅣ'
};

//종성
static const wchar_t wcTail[] = {
	L' ', L'ㄱ', L'ㄲ', L'ㄳ', L'ㄴ', L'ㄵ', L'ㄶ', L'ㄷ',	L'ㄹ', L'ㄺ', 
	L'ㄻ', L'ㄼ', L'ㄽ', L'ㄾ', L'ㄿ', L'ㅀ', L'ㅁ', L'ㅂ', L'ㅄ', L'ㅅ', 
	L'ㅆ', L'ㅇ', L'ㅈ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ'
};

// 유니코드 완성형 초성, 중성, 종성 분리
void CHangulStringUtil::SplitHangul( const wchar_t& chCompleteHangul, wchar_t& chHead, wchar_t& chMid, wchar_t& chTail)
{
	if (chCompleteHangul >= 0xAC00 && chCompleteHangul <= 0xD7A3)
	{
		// 0xAC00은 한글 코드의 시작 값이다. 따라서 저 값을 빼면  우선적으로 우리가 원하는 한글 값만을 가지게 된다.
		wchar_t chUniValue = chCompleteHangul - 0xAC00; 

		// 이제 저장한 UniValue로부터 종성을 분리한다.
		// 유니코드에서 사용가능한 초성은 19개, 중성은 21개, 종성은 28개이고
		// 유니코드 완성형 값 = ( ( ( 초성 * 21 ) + 중성 ) * 28 ) + 종성 + 0xAC00;
		// 이렇게 된다. 따라서 UniValue에서 28로 나눈 나머지는 종성이 된다.
		// 주의할 점은 저렇게 나오는 값은 인덱스 값이라는 점이다.
		// 정확한 자모 값을 받기 위해서는 유니코드 한글 자모 코드표를 통해서
		// 초성에는 초성 시작 값을 더해주고, 중성에는 중성 시작값을,
		// 종성에는 종성의 시작값을 더해주어야 정확한 자모 값을 얻을 수 있다.

		chHead = wcHead[(chUniValue) / (21 * 28)];
		chMid  = wcMid[(chUniValue)  % (21 * 28) / 28];
		chTail = wcTail[(chUniValue) % 28];
	}
}

int CHangulStringUtil::SplitHangulString(const wchar_t* pStr, wchar_t* pBuffer, UINT nSize)
{
	UINT nPos = 0;

	while (*pStr != '\0')
	{
		if (*pStr < 256)
		{
			if (nPos+2 >= nSize-1) break;

			pBuffer[nPos] = *pStr;
			++nPos;
		}
		else
		{
			if (nPos+4 >= nSize-1) break;

			SplitHangul(*pStr, pBuffer[nPos], pBuffer[nPos+1], pBuffer[nPos+2]);
			nPos+=3;
		}

		++pStr;
	}

	pBuffer[nPos] = NULL;

	return nPos;
} 

int CHangulStringUtil::GetIndex(HanType eType, wchar_t ch)

{
	int nLength;
	const wchar_t* pArray;
	switch (eType) // 초성,중성,종성 배열 : 1,2,3
	{
	case HAN_HEAD: 
		nLength = 19;
		pArray = wcHead;
		break;
	case HAN_MID: 
		nLength = 21;
		pArray = wcMid;
		break;
	case HAN_TAIL: 
		nLength = 28;
		pArray = wcTail;
		break;
	}

	int k = 0;
	for (; k < nLength; ++k, ++pArray)
	{
		if (*pArray == ch) 
			return k;
	}

	return -1; // Not Exist
}

wchar_t CHangulStringUtil::JoinHangul(wchar_t chHead, wchar_t chMid, wchar_t chTail)
{
	int nHeadIdx = GetIndex(HAN_HEAD, chHead);
	int nMidIdx  = GetIndex(HAN_MID, chMid);
	int nTailIdx = GetIndex(HAN_TAIL, chTail);

	return (nHeadIdx*21*28 + nMidIdx*28 + nTailIdx + 0xAC00);
}

void CHangulStringUtil::JoinHangulString(const wchar_t* pBuffer, wchar_t* chHan)
{   
	while (*pBuffer)
	{
		if (*pBuffer < 256) // 1바이트문자
		{
			*chHan++ = *pBuffer++;
		}
		else    // 2바이트 한글.
		{
			if (   GetIndex(HAN_MID, pBuffer[3]) == -1 
				&& pBuffer[2] >= 256)	
				// buffer[3]이 중성(모음)이 아니고 종성이 2바이트 문자면..  
			{
				*chHan = JoinHangul(pBuffer[0], pBuffer[1], pBuffer[2]);
				++chHan;

				pBuffer += 3;
			}
			else // 모음이면 종성으로 ' '보냄
			{
				*chHan = JoinHangul(pBuffer[0], pBuffer[1], L' ');
				++chHan;

				pBuffer += 2;
			}
		}
	}

	*chHan = NULL;
}
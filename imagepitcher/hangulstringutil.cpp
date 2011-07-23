#include "StdAfx.h"

#include "HangulStringUtil.h"

//�ʼ� 
static const wchar_t wcHead[] = {
	L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', 
	L'��', L'��', L'��', L'��', L'��', L'��',	L'��', L'��', L'��'
};

//�߼�
static const wchar_t wcMid[] = {
	L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', 
	L'��', L'��', L'��', L'��', L'��', L'��',	L'��', L'��', L'��', L'��', 
	L'��'
};

//����
static const wchar_t wcTail[] = {
	L' ', L'��', L'��', L'��', L'��', L'��', L'��', L'��',	L'��', L'��', 
	L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', 
	L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��'
};

// �����ڵ� �ϼ��� �ʼ�, �߼�, ���� �и�
void CHangulStringUtil::SplitHangul( const wchar_t& chCompleteHangul, wchar_t& chHead, wchar_t& chMid, wchar_t& chTail)
{
	if (chCompleteHangul >= 0xAC00 && chCompleteHangul <= 0xD7A3)
	{
		// 0xAC00�� �ѱ� �ڵ��� ���� ���̴�. ���� �� ���� ����  �켱������ �츮�� ���ϴ� �ѱ� ������ ������ �ȴ�.
		wchar_t chUniValue = chCompleteHangul - 0xAC00; 

		// ���� ������ UniValue�κ��� ������ �и��Ѵ�.
		// �����ڵ忡�� ��밡���� �ʼ��� 19��, �߼��� 21��, ������ 28���̰�
		// �����ڵ� �ϼ��� �� = ( ( ( �ʼ� * 21 ) + �߼� ) * 28 ) + ���� + 0xAC00;
		// �̷��� �ȴ�. ���� UniValue���� 28�� ���� �������� ������ �ȴ�.
		// ������ ���� ������ ������ ���� �ε��� ���̶�� ���̴�.
		// ��Ȯ�� �ڸ� ���� �ޱ� ���ؼ��� �����ڵ� �ѱ� �ڸ� �ڵ�ǥ�� ���ؼ�
		// �ʼ����� �ʼ� ���� ���� �����ְ�, �߼����� �߼� ���۰���,
		// �������� ������ ���۰��� �����־�� ��Ȯ�� �ڸ� ���� ���� �� �ִ�.

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
	switch (eType) // �ʼ�,�߼�,���� �迭 : 1,2,3
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
		if (*pBuffer < 256) // 1����Ʈ����
		{
			*chHan++ = *pBuffer++;
		}
		else    // 2����Ʈ �ѱ�.
		{
			if (   GetIndex(HAN_MID, pBuffer[3]) == -1 
				&& pBuffer[2] >= 256)	
				// buffer[3]�� �߼�(����)�� �ƴϰ� ������ 2����Ʈ ���ڸ�..  
			{
				*chHan = JoinHangul(pBuffer[0], pBuffer[1], pBuffer[2]);
				++chHan;

				pBuffer += 3;
			}
			else // �����̸� �������� ' '����
			{
				*chHan = JoinHangul(pBuffer[0], pBuffer[1], L' ');
				++chHan;

				pBuffer += 2;
			}
		}
	}

	*chHan = NULL;
}
#include "StdAfx.h"

#include "shlobj.h"
#include "AppDataFile.h"
#include "EncryptionManager.h"
#include "StringUtil.h"

#define ROOT_NODE_NAME		_T("Main")

struct CAppDataFile::AppDataFileInfo
{
	tstring strAppData;
	tstring strAppName;
	tstring strFileName;
};

using namespace rapidxml;

CAppDataFile* CAppDataFile::m_pAppDataFile = nullptr;

CAppDataFile::CAppDataFile()
{
	m_pInfo = new AppDataFileInfo;
}

CAppDataFile::~CAppDataFile()
{
	SAFE_DELETE(m_pInfo);
}

CAppDataFile* CAppDataFile::GetInstance()
{
	if (m_pAppDataFile == nullptr)
	{
		m_pAppDataFile = new CAppDataFile;
	}

	return m_pAppDataFile;
}

void CAppDataFile::ReleaseInstance()
{
	SAFE_DELETE(m_pAppDataFile);
}

// %USER%\APPNAME 에 있는걸 읽는다.

void CAppDataFile::Init(const tstring& strAppName)
{
	m_pInfo->strAppName = strAppName;
	m_pInfo->strFileName = strAppName + _T(".xml");

	TCHAR szAppData[MAX_PATH] = { 0, };
	::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
	tstring strAppData(szAppData);

	if (!strAppData.empty())
	{
		m_pInfo->strAppData = strAppData + _T("\\") + strAppName;

		strAppData += _T("\\") + strAppName;
		::CreateDirectory(strAppData.c_str(), NULL);
	}
}

void CAppDataFile::Init(const tstring& strAppName, const tstring& strPrefix)
{
	m_pInfo->strAppName = strAppName;
	m_pInfo->strFileName = strAppName + _T(".xml");
	m_pInfo->strAppData = strPrefix;
}

tstring	CAppDataFile::LoadAppDataFile(const tstring& strFileName)
{
	if (strFileName.empty())			return _T("");
	if (m_pInfo->strAppData.empty())	return _T("");

	tstring strPath = m_pInfo->strAppData + _T("\\") + strFileName;

	tstring strData;
	CEncryptionManager::LoadFile(strPath, strData, NULL);
	CStringUtil::Trim(strData, _T(" "));

	return strData;
}

void CAppDataFile::SaveAppDataFile(const tstring& strFileName, const tstring& strXml, bool bIsEncryption)
{
	if (m_pInfo->strAppData.empty())		
		return;

	xml_document<TCHAR> xmlDoc;
	xmlDoc.parse<0>(const_cast<TCHAR*>(strXml.c_str()));

	tstring strSaveXml;
	print(std::back_inserter(strSaveXml), xmlDoc, 0);

	tstring strPath = m_pInfo->strAppData + _T("\\") + strFileName;

	CEncryptionManager::SaveFile(strPath, strSaveXml, bIsEncryption, _T("UTF-8"));
}

int CAppDataFile::GetAppDataInt(const tstring& strSection, const tstring& strName, int nDefault)
{
	tstring strData = GetAppData(strSection, strName);
	if (strData.empty())		
		return nDefault;

	return _ttoi(strData.c_str());
}

void CAppDataFile::SetAppDataInt(const tstring& strSection, const tstring& strName, int nValue)
{
	SetAppData(strSection, strName, CStringUtil::IntToStr(nValue));
}

tstring	CAppDataFile::GetAppData(const tstring& strSection, const tstring& strName, const tstring& strDefault)
{
	tstring strData = LoadAppDataFile(m_pInfo->strFileName);

	try
	{
		if (strData.empty())			
			return strDefault;

		xml_document<TCHAR> xmlDoc;
		xmlDoc.parse<0>(const_cast<TCHAR*>(strData.c_str()));

		xml_node<TCHAR>* pRoot = xmlDoc.first_node();
		if (!pRoot)				
			return strDefault;

		xml_node<TCHAR>* pSection = NULL;
		for (xml_node<TCHAR>* pItem = pRoot->first_node(); pItem; pItem = pItem->next_sibling())
		{
			if (pItem->name() == strSection)
			{
				pSection = pItem;
				break;
			}
		}

		if (!pSection)		
			return strDefault;

		xml_node<TCHAR>* pNode = NULL;
		for (xml_node<TCHAR>* pItem = pSection->first_node(); pItem; pItem = pItem->next_sibling())
		{
			if (pItem->name() == strName)
			{
				pNode = pItem;
				break;
			}
		}

		if (!pNode)				
			return strDefault;

		return pNode->value();
	}
	catch (...)
	{
	}

	return strDefault;
}

void CAppDataFile::SetAppData(const tstring& strSection, const tstring& strName, const tstring& strValue)
{
	tstring strData = LoadAppDataFile(m_pInfo->strFileName);

	xml_document<TCHAR> xmlDoc;

	if (strData.empty())
	{
		xml_node<TCHAR>* pRoot = xmlDoc.allocate_node(node_element, ROOT_NODE_NAME);
		xmlDoc.append_node(pRoot);

		xml_node<TCHAR>* pSection = xmlDoc.allocate_node(node_element, strSection.c_str());
		pRoot->append_node(pSection);

		xml_node<TCHAR>* pNode = xmlDoc.allocate_node(node_element, strName.c_str());
		pSection->append_node(pNode);

		pNode->value(strValue.c_str());
	}
	else
	{
		xmlDoc.parse<0>(const_cast<TCHAR*>(strData.c_str()));

		xml_node<TCHAR>* pRoot = xmlDoc.first_node();
		if (!pRoot)
			return;

		xml_node<TCHAR>* pSection = NULL;
		for (xml_node<TCHAR>* pItem = pRoot->first_node(); pItem; pItem = pItem->next_sibling())
		{
			if (pItem->name() == strSection)
			{
				pSection = pItem;
				break;
			}
		}

		if (!pSection)
		{
			pSection = xmlDoc.allocate_node(node_element, strSection.c_str());
			if (pSection)	
				pRoot->append_node(pSection);
			else				
				return;
		}
		
		xml_node<TCHAR>* pNode = NULL;
		for (xml_node<TCHAR>* pItem = pSection->first_node(); pItem; pItem = pItem->next_sibling())
		{
			if (pItem->name() == strName)
			{
				pNode = pItem;
				break;
			}
		}
		
		// 그냥 value로 바꾸기만 하면 적용이 안되므로 삭제후 재할당한다.
		if (pNode)
		{
			pSection->remove_node(pNode);
		}

		pNode = xmlDoc.allocate_node(node_element, strName.c_str());

		if (pNode)		
		{
			pNode->value(strValue.c_str());
			pSection->append_node(pNode);
		}
		else			
			return;
				
	}

	tstring strPrintXml;
	print(std::back_inserter(strPrintXml), xmlDoc, 0);
	SaveAppDataFile(m_pInfo->strFileName, strPrintXml, FALSE);
}

tstring	CAppDataFile::GetAppDataPath()
{
	return m_pInfo->strAppData;
}
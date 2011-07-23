#pragma once

class CAppDataFile
{
public:
	~CAppDataFile();

	static CAppDataFile* GetInstance();
	static void ReleaseInstance();

	void	Init(const tstring& strAppName);
	void	Init(const tstring& strAppName, const tstring& strPrefix);
	
	tstring	LoadAppDataFile(const tstring& strFileName);
	void	SaveAppDataFile(const tstring& strFileName, const tstring& strXml, bool bIsEncryption = false);

	int		GetAppDataInt(const tstring& strSection, const tstring& strName, int nDefault = 0);
	void	SetAppDataInt(const tstring& strSection, const tstring& strName, int nValue);
	tstring	GetAppData(const tstring& strSection, const tstring& strName, const tstring& strDefault = _T(""));
	void	SetAppData(const tstring& strSection, const tstring& strName, const tstring& strValue);

	tstring	GetAppDataPath();

private:
	CAppDataFile();

protected:
	static CAppDataFile* m_pAppDataFile;

	struct AppDataFileInfo;
	AppDataFileInfo* m_pInfo;
};

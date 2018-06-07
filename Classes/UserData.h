#pragma once

#include "Includes.h"

class CUserData 
{
private:
	int m_nID;
	std::string m_szName;

	PLUnitIdx m_userPLIdx1;
	PLUnitIdx m_userPLIdx2;
	
	std::vector<bool> m_isCharsUnlock;
	std::vector<bool> m_isStagesUnlock;
public:
	CUserData();
	virtual ~CUserData();
public:
	void saveData();
	void loadData();
public:
	void initUserData(std::string &Name);
private:
	bool parseJson(std::string &json);
	std::string createJson();
public:
	int getId();
	const std::string& getName() const;

	PLUnitIdx getUserPLIDX1();
	PLUnitIdx getUserPLIDX2();
	const std::vector<bool> &getCurGetChars() const;
	const std::vector<bool> &getCurUnlockStages() const;

	void setUserPLIDXs(PLUnitIdx idx1, PLUnitIdx idx2);
	void setIsGetChar(bool isGet, int nIdx);
	void setIsUnlockStage(bool isClear, int nIdx);
};
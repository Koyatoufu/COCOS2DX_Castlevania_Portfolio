#pragma once

#include "Includes.h"

class CStageData 
{
private:
	std::vector<StageData> m_stageDatas;
public:
	CStageData();
	~CStageData();
public:
	void loadData();
	bool parseJson(std::string &json);
public:
	const StageData& getStageData(unsigned int nIdx)const;
};
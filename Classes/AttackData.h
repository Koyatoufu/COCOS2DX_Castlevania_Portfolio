#pragma once

#include "Includes.h"

class CAttackData
{
private:
	std::vector<std::vector<std::vector<AttackData>>> m_data;
public:
	CAttackData();
	~CAttackData();
public:
	void loadData();
	bool parseJson(std::string &json);
	AttackData& getAttackData(int nCharType, int nIdx, int nAtkIdx);
};
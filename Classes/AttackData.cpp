#include "AttackData.h"

#include "json\rapidjson.h"
#include "json\document.h"
#include "json\stringbuffer.h"

using namespace rapidjson;

CAttackData::CAttackData():m_data()
{
	loadData();
}

CAttackData::~CAttackData()
{
}

void CAttackData::loadData()
{
	auto pUtil = FileUtils::getInstance();

	std::string json = pUtil->getStringFromFile("data/attackdata.json");

	CC_ASSERT(parseJson(json));
}

bool CAttackData::parseJson(std::string & json)
{
	if (json.empty())
		return false;

	Document document;
	document.Parse<0>(json.c_str());

	if (document.HasParseError())
		return false;

	if (document.IsArray())
	{
		m_data.reserve(document.Capacity());
		m_data.resize(document.Size());

		for (int i = 0; i < document.Size(); i++)
		{
			m_data[i].reserve(document[i].Capacity());
			m_data[i].resize(document[i].Size());
			for (int j = 0; j < document[i].Size(); j++)
			{
				m_data[i][j].reserve(document[i][j].Capacity());
				m_data[i][j].resize(document[i][j].Size());
				for (int k = 0; k < document[i][j].Size(); k++)
				{
					m_data[i][j][k].nAtk = document[i][j][k]["atk"].GetInt();
					m_data[i][j][k].option = (HitOption)document[i][j][k]["hitOption"].GetInt();
					m_data[i][j][k].type = (AttackType)document[i][j][k]["type"].GetInt();
					m_data[i][j][k].nEffIdx = document[i][j][k]["effect"].GetInt();
				}
			}
		}
	}

	return true;
}

AttackData & CAttackData::getAttackData(int nCharType, int nIdx, int nAtkIdx)
{
	return m_data[nCharType][nIdx][nAtkIdx];
}

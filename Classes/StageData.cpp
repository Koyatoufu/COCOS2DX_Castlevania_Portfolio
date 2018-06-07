#include "StageData.h"

#include "json\rapidjson.h"
#include "json\document.h"

using namespace rapidjson;

CStageData::CStageData()
{
	loadData();
}

CStageData::~CStageData()
{
}

void CStageData::loadData()
{
	auto pUtil = FileUtils::getInstance();

	std::string json = pUtil->getStringFromFile("data/stagedata.json");

	CC_ASSERT(parseJson(json));
}

bool CStageData::parseJson(std::string & json)
{
	if (json.empty())
		return false;

	Document document;
	document.Parse<0>(json.c_str());

	if (document.HasParseError())
		return false;

	if (document.IsArray())
	{
		m_stageDatas.reserve(document.Capacity());
		m_stageDatas.resize(document.Size());

		for (unsigned int i = 0; i < document.Size(); i++)
		{
			m_stageDatas[i].name = document[i]["name"].GetString();
			m_stageDatas[i].bgmPath = document[i]["bgm"].GetString();
			m_stageDatas[i].size.width = document[i]["width"].GetFloat();
			m_stageDatas[i].size.height = document[i]["height"].GetFloat();

			if (document[i].HasMember("roomData")&&document[i]["roomData"].IsArray())
			{
				const rapidjson::Value& arr = document[i]["roomData"].GetArray();

				m_stageDatas[i].roomDatas.reserve(arr.Capacity());
				m_stageDatas[i].roomDatas.resize(arr.Size());

				for (unsigned int j = 0; j < arr.Size(); j++)
				{
					m_stageDatas[i].roomDatas[j].size.width = arr[j]["width"].GetFloat();
					m_stageDatas[i].roomDatas[j].size.height = arr[j]["height"].GetFloat();
					m_stageDatas[i].roomDatas[j].position.x = arr[j]["x"].GetFloat();
					m_stageDatas[i].roomDatas[j].position.y = arr[j]["y"].GetFloat();
					m_stageDatas[i].roomDatas[j].isUseParallax = arr[j]["useParallax"].GetBool();
					m_stageDatas[i].roomDatas[j].nEnemyMin = arr[j]["enemyMin"].GetInt();
					m_stageDatas[i].roomDatas[j].nObstacleMin = arr[j]["obstacleMin"].GetInt();
					m_stageDatas[i].roomDatas[j].nEnemyRand = arr[j]["enemyRand"].GetInt();
					m_stageDatas[i].roomDatas[j].nObstacleRand = arr[j]["obstacleRand"].GetInt();
				}
			}

			if (document[i].HasMember("enemyIdx") && document[i]["enemyIdx"].IsArray())
			{
				const rapidjson::Value& arr = document[i]["enemyIdx"].GetArray();

				m_stageDatas[i].enemyIdxes.reserve(arr.Capacity());
				m_stageDatas[i].enemyIdxes.resize(arr.Size());

				for (unsigned int j = 0; j < arr.Size(); j++)
				{
					m_stageDatas[i].enemyIdxes[j] = arr[j].GetInt();
				}
			}

			m_stageDatas[i].nObstacleIdx = document[i]["obstacleIdx"].GetInt();

			if (document[i].HasMember("unlockable") && document[i]["unlockable"].IsObject())
			{
				m_stageDatas[i].unlockChar = (PLUnitIdx)document[i]["unlockable"]["char"].GetInt();

				if (document[i]["unlockable"].HasMember("stage") && document[i]["unlockable"]["stage"].IsArray())
				{
					const rapidjson::Value& arr = document[i]["unlockable"]["stage"].GetArray();

					m_stageDatas[i].unlockStages.reserve(arr.Capacity());
					m_stageDatas[i].unlockStages.resize(arr.Size());

					for (unsigned int j = 0; j < arr.Size(); j++)
					{
						m_stageDatas[i].unlockStages[j] = arr[j].GetInt();
					}
				}
			}
		}
	}

	return true;
}

const StageData& CStageData::getStageData(unsigned int nIdx)const
{
	return m_stageDatas[nIdx];
}

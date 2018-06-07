#include "ObjectData.h"

#include "json\rapidjson.h"
#include "json\document.h"

using namespace rapidjson;

CObjectData::CObjectData()
{
	loadData();
}

CObjectData::~CObjectData()
{
}

void CObjectData::loadData()
{
	auto pUtil = FileUtils::getInstance();

	std::string json = pUtil->getStringFromFile("data/objectdata.json");

	CC_ASSERT(parseJson(json));
}

bool CObjectData::parseJson(std::string & json)
{
	if (json.empty())
		return false;

	Document document;
	document.Parse<0>(json.c_str());

	if (document.HasParseError())
		return false;

	if (document.HasMember("Unit") && document["Unit"].IsObject())
		InitUnitDatas(document);

	if (document.HasMember("Item") && document["Item"].IsArray())
	{
		const rapidjson::Value& arr = document["Item"].GetArray();
		
		m_itemDatas.reserve(arr.Capacity());
		m_itemDatas.resize(arr.Size());
		for (int i = 0; i < arr.Size(); i++)
		{
			m_itemDatas[i].filePath = arr[i]["filepath"].GetString();
			m_itemDatas[i].type = (ItemType)arr[i]["type"].GetInt();
			m_itemDatas[i].nValue = arr[i]["value"].GetInt();
			m_itemDatas[i].nSeIdx = arr[i]["se"].GetInt();
		}
	}

	if (document.HasMember("Weapon") && document["Weapon"].IsArray())
	{
		const rapidjson::Value& arr = document["Weapon"].GetArray();
		
		m_weaponDatas.reserve(arr.Capacity());
		m_weaponDatas.resize(arr.Size());
		for (int i = 0; i < arr.Size(); i++)
		{
			m_weaponDatas[i].filePath = arr[i]["filepath"].GetString();
			m_weaponDatas[i].type = (WeaponType)arr[i]["type"].GetInt();
			m_weaponDatas[i].nAtk = arr[i]["atk"].GetInt();
		}
	}

	if (document.HasMember("Projectile") && document["Projectile"].IsArray())
	{
		const rapidjson::Value& arr = document["Projectile"].GetArray();

		m_projectileDatas.reserve(arr.Capacity());
		m_projectileDatas.resize(arr.Size());

		for (int i = 0; i < arr.Size(); i++)
		{
			m_projectileDatas[i].filePath = arr[i]["filepath"].GetString();
			m_projectileDatas[i].iconPath = arr[i]["icon"].GetString();

			m_projectileDatas[i].type = (ProjectileType)arr[i]["type"].GetInt();
			m_projectileDatas[i].fCost = arr[i]["cost"].GetFloat();
			m_projectileDatas[i].fMoveSpeed = arr[i]["movespeed"].GetFloat();
			m_projectileDatas[i].fLiveTime = arr[i]["livetime"].GetFloat();
			m_projectileDatas[i].fLength = arr[i]["length"].GetFloat();
			m_projectileDatas[i].fAirForce = arr[i]["airForce"].GetFloat();
			m_projectileDatas[i].nSEIdx = arr[i]["se"].GetInt();

			if (arr[i].HasMember("atkData") && arr[i]["atkData"].IsObject())
			{
				m_projectileDatas[i].atkData.nAtk = arr[i]["atkData"]["atk"].GetInt();
				m_projectileDatas[i].atkData.type = (AttackType)arr[i]["atkData"]["atkType"].GetInt();
				m_projectileDatas[i].atkData.option = (HitOption)arr[i]["atkData"]["hitOption"].GetInt();
				m_projectileDatas[i].atkData.nEffIdx = arr[i]["atkData"]["effect"].GetInt();
			}
		}
	}

	if (document.HasMember("Effect") && document["Effect"].IsArray())
	{
		const rapidjson::Value& arr = document["Effect"].GetArray();

		m_effectDatas.reserve(arr.Capacity());
		m_effectDatas.resize(arr.Size());

		for (int i = 0; i < arr.Size(); i++)
		{
			m_effectDatas[i].filePath = arr[i]["filepath"].GetString();
			m_effectDatas[i].fLiveTime = arr[i]["liveTime"].GetFloat();
			m_effectDatas[i].isGravity = arr[i]["isGravity"].GetBool();
			m_effectDatas[i].nSeIdx = arr[i]["se"].GetInt();
		}
	}

	if (document.HasMember("Obstacle") && document["Obstacle"].IsArray())
	{
		const rapidjson::Value& arr = document["Obstacle"].GetArray();

		m_obstacleDatas.reserve(arr.Capacity());
		m_obstacleDatas.resize(arr.Size());

		for (int i = 0; i < arr.Size(); i++)
		{
			m_obstacleDatas[i].filePath = arr[i]["filepath"].GetString();
			m_obstacleDatas[i].hpData = ObjHpData(arr[i]["status"]["hp"].GetFloat(), arr[i]["status"]["mp"].GetFloat());
		}
	}

	return true;
}

void CObjectData::InitUnitDatas(rapidjson::Document & document)
{
	if (document["Unit"].HasMember("Player") && document["Unit"]["Player"].IsArray())
	{
		const rapidjson::Value& arr = document["Unit"]["Player"].GetArray();
		m_plUnitDatas.reserve(arr.Capacity());
		m_plUnitDatas.resize(arr.Size());
		for (unsigned int i = 0; i < arr.Size(); i++)
		{
			m_plUnitDatas[i].filePath = arr[i]["filepath"].GetString();
			m_plUnitDatas[i].name = arr[i]["name"].GetString();

			m_plUnitDatas[i].fMoveSpeed = arr[i]["movespeed"].GetFloat();

			m_plUnitDatas[i].hpData = ObjHpData(arr[i]["status"]["hp"].GetFloat(), arr[i]["status"]["mp"].GetFloat());
			m_plUnitDatas[i].status = UnitStatus(arr[i]["status"]["str"].GetInt(), arr[i]["status"]["con"].GetInt(),
				arr[i]["status"]["int"].GetInt(), arr[i]["status"]["reg"].GetInt());

			m_plUnitDatas[i].weaponType = (WeaponType)arr[i]["wType"].GetInt();
			m_plUnitDatas[i].nWeaponIdx = arr[i]["wIdx"].GetInt();

			auto arSwIdx = arr[i]["sWIdx"].GetArray();
			m_plUnitDatas[i].subWeaponIdx.reserve(arSwIdx.Capacity());
			m_plUnitDatas[i].subWeaponIdx.resize(arSwIdx.Size());

			for (unsigned int j = 0; j < arSwIdx.Size(); j++)
			{
				m_plUnitDatas[i].subWeaponIdx[j] = arSwIdx[j].GetInt();
			}
		}
	}

	if (document["Unit"].HasMember("Enemy") && document["Unit"]["Enemy"].IsArray())
	{
		const rapidjson::Value& arr = document["Unit"]["Enemy"].GetArray();
		m_enemyUnitDatas.reserve(arr.Capacity());
		m_enemyUnitDatas.resize(arr.Size());
		for (unsigned int i = 0; i < arr.Size(); i++)
		{
			m_enemyUnitDatas[i].filePath = arr[i]["filepath"].GetString();

			m_enemyUnitDatas[i].name = arr[i]["name"].GetString();
			m_enemyUnitDatas[i].type = (EnemyType)arr[i]["type"].GetInt();

			m_enemyUnitDatas[i].fMoveSpeed = arr[i]["movespeed"].GetFloat();

			m_enemyUnitDatas[i].nProjtileIdx = arr[i]["projectile"].GetInt();

			m_enemyUnitDatas[i].hpData = ObjHpData(arr[i]["status"]["hp"].GetFloat(), arr[i]["status"]["mp"].GetFloat());
			m_enemyUnitDatas[i].status = UnitStatus(arr[i]["status"]["str"].GetInt(), arr[i]["status"]["con"].GetInt(),
				arr[i]["status"]["int"].GetInt(), arr[i]["status"]["reg"].GetInt());
		}
	}
}

PlayerUnitData CObjectData::getPlayerUnitData(unsigned int nIdx)
{
	if (nIdx >= (int)m_plUnitDatas.size())
	{
		if (nIdx <= 0)
			return PlayerUnitData();
		nIdx = m_plUnitDatas.size() - 1;
	}

	return m_plUnitDatas[nIdx];
}

EnemyUnitData CObjectData::getEnemyUnitData(unsigned int nIdx)
{
	if (nIdx >= m_enemyUnitDatas.size())
	{
		if (nIdx <= 0)
			return EnemyUnitData();
		nIdx = m_enemyUnitDatas.size() - 1;
	}

	return m_enemyUnitDatas[nIdx];
}

ItemData CObjectData::getItemData(unsigned int nIdx)
{
	if (nIdx >= m_itemDatas.size())
	{
		if (nIdx <= 0)
			return ItemData();
		nIdx = m_itemDatas.size() - 1;
	}

	return m_itemDatas[nIdx];
}

ProjectileData CObjectData::getProjectileData(unsigned int nIdx)
{
	if (nIdx >= m_projectileDatas.size())
	{
		if (nIdx>=100)
			return ProjectileData();
		nIdx = m_projectileDatas.size() - 1;
	}

	return m_projectileDatas[nIdx];
}

WeaponData CObjectData::getWeaponData(unsigned int nIdx)
{
	if (nIdx >= m_weaponDatas.size())
	{
		if (nIdx <= 0)
			return WeaponData();
		nIdx = m_weaponDatas.size() - 1;
	}
	return m_weaponDatas[nIdx];
}

EffectData CObjectData::getEffectData(unsigned int nIdx)
{
	if (nIdx >= m_effectDatas.size())
	{
		if (nIdx <= 0)
			return EffectData();
		nIdx = m_effectDatas.size() - 1;
	}

	return m_effectDatas[nIdx];
}

ObstacleData CObjectData::getObstacleData(unsigned int nIdx)
{
	if (nIdx >= m_obstacleDatas.size())
	{
		if (nIdx <= 0)
			return ObstacleData();
		nIdx = m_obstacleDatas.size() - 1;
	}

	return m_obstacleDatas[nIdx];
}

unsigned int CObjectData::getItemDataSize()
{
	return m_itemDatas.size();
}

unsigned int CObjectData::getProjectileDataSize()
{
	return m_projectileDatas.size();
}

unsigned int CObjectData::getEffectDataSize()
{
	return m_effectDatas.size();
}

unsigned int CObjectData::getObstacleDataSize()
{
	return m_obstacleDatas.size();
}

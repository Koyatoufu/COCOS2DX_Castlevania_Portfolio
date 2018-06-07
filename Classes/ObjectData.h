#pragma once

#include "Includes.h"

class CObjectData
{
private:
	std::vector<PlayerUnitData> m_plUnitDatas;
	std::vector<EnemyUnitData> m_enemyUnitDatas;
	std::vector<ItemData> m_itemDatas;
	std::vector<ProjectileData> m_projectileDatas;
	std::vector<WeaponData> m_weaponDatas;
	std::vector<EffectData> m_effectDatas;
	std::vector<ObstacleData> m_obstacleDatas;
public:
	CObjectData();
	virtual ~CObjectData();
public:
	void loadData();
private:
	bool parseJson(std::string &json);
	void InitUnitDatas(rapidjson::Document& document);
public:
	PlayerUnitData getPlayerUnitData(unsigned int nIdx);
	EnemyUnitData getEnemyUnitData(unsigned int nIdx);
	ItemData getItemData(unsigned int nIdx);
	ProjectileData getProjectileData(unsigned int nIdx);
	WeaponData getWeaponData(unsigned int nIdx);
	EffectData getEffectData(unsigned int nIdx);
	ObstacleData getObstacleData(unsigned int nIdx);
public:
	unsigned int getItemDataSize();
	unsigned int getProjectileDataSize();
	unsigned int getEffectDataSize();
	unsigned int getObstacleDataSize();
};
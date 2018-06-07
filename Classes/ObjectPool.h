#pragma once

#include "Object.h"

class CObjectData;

class CObjectPool 
{
private:
	struct PoolData
	{
		std::vector<CObject*> pools;
		bool isMountable;
	};
private:
	static CObjectPool* m_pInst;

	std::map<ObjectType,std::map<unsigned int,PoolData>> m_poolDatas;
	int m_nAmountSize;
private:
	CObjectPool();
	~CObjectPool();
public:
	static CObjectPool* getInst();
	static void releaseInst();

	void createPool(unsigned int nCurStage);

	CObject* getObject(ObjectType type,unsigned int nIdx);
	void PoolObject(CObject* pData, ObjectType type, unsigned int nIdx);
private:
	void createEnemyPool(CObjectData& objdata, StageData& stageData);
	void createItemPool(CObjectData& objdata);
	void createProjectilePool(CObjectData& objdata);
	void createEffectPool(CObjectData& objdata);
	void createObstaclePool(CObjectData& objdata, StageData& stageData);
};
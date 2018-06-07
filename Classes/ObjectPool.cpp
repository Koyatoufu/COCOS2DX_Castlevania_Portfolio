#include "ObjectPool.h"
#include "DataMgr.h"
#include "EnemyRunner.h"
#include "EnemyShooter.h"
#include "Boss.h"
#include "Obstacle.h"
#include "Projectile.h"
#include "Effect.h"
#include "Item.h"

CObjectPool* CObjectPool::m_pInst;

CObjectPool::CObjectPool():m_poolDatas(),m_nAmountSize(20)
{
}

CObjectPool::~CObjectPool()
{
	for (unsigned int i = 0; i < (int)ObjectType::MAX; i++)
	{
		for (unsigned int j = 0; j < m_poolDatas[(ObjectType)i].size(); j++)
		{
			for (unsigned int k = 0; k < m_poolDatas[(ObjectType)i][j].pools.size(); k++)
			{
				m_poolDatas[(ObjectType)i][j].pools[k]->release();
			}
		}
	}
}

CObjectPool * CObjectPool::getInst()
{
	if (!m_pInst)
	{
		m_pInst = new CObjectPool();

	}
	return m_pInst;
}

void CObjectPool::releaseInst()
{
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}

void CObjectPool::createPool(unsigned int nCurStage)
{
	auto objectData = CDataMgr::getInst()->getObjData();
	auto stageData = CDataMgr::getInst()->getStageData()->getStageData(nCurStage);

	createEnemyPool(*objectData, stageData);
	createItemPool(*objectData);
	createEffectPool(*objectData);
	createProjectilePool(*objectData);
	createObstaclePool(*objectData, stageData);
}

CObject * CObjectPool::getObject(ObjectType type, unsigned int nIdx)
{
	int nPoolSize = m_poolDatas[type][nIdx].pools.size();

	CObject* pObject = nullptr;

	if (nPoolSize <= 0)
	{
		if (m_poolDatas[type][nIdx].isMountable)
		{
			auto objectData = CDataMgr::getInst()->getObjData();

			switch (type)
			{
			case ObjectType::UNIT:
			{
				EnemyType enemyType = objectData->getEnemyUnitData(nIdx).type;

				switch (enemyType)
				{
				case EnemyType::RUNNER:
					pObject = CEnemyRunner::create(nIdx);
					break;
				case EnemyType::SHOOTER:
					pObject = CEnemyShooter::create(nIdx);
					break;
				//case EnemyType::BRAWER:
				//	break;
				case EnemyType::BOSS:
					pObject = CBoss::create(nIdx);
					break;
				default:
					pObject = CEnemyRunner::create(0);
					break;
				}
			}
			break;
			case ObjectType::ITEM:
				pObject = CItem::create(nIdx);
				break;
			case ObjectType::PROJECTILE:
				pObject = CProjectile::create(nIdx);
				break;
			case ObjectType::OBSTACLE:
				pObject = CObstacle::create(nIdx);
				break;
			case ObjectType::EFFECT:
				pObject = CEffect::create(nIdx);
				break;
			default:
				return pObject;
			}
		}

		if (!pObject)
			return nullptr;

		pObject->retain();
		pObject->setVisible(true);
		return pObject;
	}

	pObject = m_poolDatas[type][nIdx].pools[nPoolSize - 1];
	//pObject->setVisible(true);
	m_poolDatas[type][nIdx].pools.pop_back();
	nPoolSize = m_poolDatas[type][nIdx].pools.size();

	return pObject;
}

void CObjectPool::PoolObject(CObject * pData, ObjectType type, unsigned int nIdx)
{
	int nPoolSize = m_poolDatas[type][nIdx].pools.size();

	if (nPoolSize >= m_nAmountSize)
	{
		if (!m_poolDatas[type][nIdx].isMountable)
		{
			pData->removeFromParentAndCleanup(true);
			//pData->release();
			return;
		}
	}

	pData->removeFromParentAndCleanup(false);
	pData->retain();
	pData->setVisible(false);
	m_poolDatas[type][nIdx].pools.push_back(pData);


}

void CObjectPool::createEnemyPool(CObjectData & objdata, StageData & stageData)
{
	std::map<unsigned int, PoolData> unitMap;

	for (unsigned int i = 0; i < stageData.enemyIdxes.size(); i++)
	{
		unsigned int nEnemyIdx = stageData.enemyIdxes[i];
		EnemyUnitData enemyData = objdata.getEnemyUnitData(nEnemyIdx);

		//map.at(i).pools.reserve(m_nAmountSize);
		unitMap[nEnemyIdx].pools.reserve(m_nAmountSize);
		unitMap[nEnemyIdx].pools.resize(m_nAmountSize);
		unitMap[nEnemyIdx].isMountable = false;

		for (int j = 0; j < m_nAmountSize; j++)
		{
			switch (enemyData.type)
			{
			case EnemyType::RUNNER:
				unitMap[nEnemyIdx].pools[j] = CEnemyRunner::create(nEnemyIdx);
				break;
			case EnemyType::SHOOTER:
				unitMap[nEnemyIdx].pools[j] = CEnemyShooter::create(nEnemyIdx);
				break;
			//case EnemyType::BRAWER:
			//	break;
			case EnemyType::BOSS:
				unitMap[nEnemyIdx].pools[j] = CBoss::create(nEnemyIdx);
				break;
			default:
				unitMap[nEnemyIdx].pools[j] = CEnemyRunner::create(0);
				break;
			}

			unitMap[nEnemyIdx].pools[j]->retain();
			unitMap[nEnemyIdx].pools[j]->setVisible(false);
		}

	}

	m_poolDatas[ObjectType::UNIT] = unitMap;
}

void CObjectPool::createItemPool(CObjectData & objdata)
{
	std::map<unsigned int, PoolData> itemMap;
	
	for (unsigned int i = 0; i < objdata.getItemDataSize(); i++)
	{
		itemMap[i].pools.reserve(m_nAmountSize);
		itemMap[i].pools.resize(m_nAmountSize);
		itemMap[i].isMountable = true;

		for (int j = 0; j < m_nAmountSize; j++)
		{
			itemMap[i].pools[j] = CItem::create(i);
			itemMap[i].pools[j]->retain();
			itemMap[i].pools[j]->setVisible(false);
		}
	}

	m_poolDatas[ObjectType::ITEM] = itemMap;
}

void CObjectPool::createProjectilePool(CObjectData & objdata)
{
	std::map<unsigned int, PoolData> projectileMap;

	for (unsigned int i = 0; i < objdata.getItemDataSize(); i++)
	{
		projectileMap[i].pools.reserve(m_nAmountSize);
		projectileMap[i].pools.resize(m_nAmountSize);

		for (int j = 0; j < m_nAmountSize; j++)
		{
			projectileMap[i].pools[j] = CProjectile::create(i);
			projectileMap[i].pools[j]->retain();
			projectileMap[i].pools[j]->setVisible(false);
		}
	}

	m_poolDatas[ObjectType::PROJECTILE] = projectileMap;
}

void CObjectPool::createEffectPool(CObjectData & objdata)
{
	std::map<unsigned int, PoolData> effectMap;

	for (unsigned int i = 0; i < objdata.getItemDataSize(); i++)
	{
		effectMap[i].pools.reserve(m_nAmountSize);
		effectMap[i].pools.resize(m_nAmountSize);
		effectMap[i].isMountable = true;

		for (int j = 0; j < m_nAmountSize; j++)
		{
			effectMap[i].pools[j] = CEffect::create(i);
			effectMap[i].pools[j]->retain();
			effectMap[i].pools[j]->setVisible(false);
		}
	}

	m_poolDatas[ObjectType::EFFECT] = effectMap;
}

void CObjectPool::createObstaclePool(CObjectData & objdata, StageData & stageData)
{
	std::map<unsigned int, PoolData> obstacleMap;

	for (unsigned int i = 0; i < stageData.enemyIdxes.size(); i++)
	{
		unsigned int nIdx = stageData.nObstacleIdx;
		
		obstacleMap[nIdx].pools.reserve(m_nAmountSize);
		obstacleMap[nIdx].pools.resize(m_nAmountSize);
		obstacleMap[nIdx].isMountable = false;

		for (int j = 0; j < m_nAmountSize; j++)
		{
			obstacleMap[nIdx].pools[j] = CObstacle::create(nIdx);
			obstacleMap[nIdx].pools[j]->retain();
			obstacleMap[nIdx].pools[j]->setVisible(false);
		}

	}

	m_poolDatas[ObjectType::OBSTACLE] = obstacleMap;
}



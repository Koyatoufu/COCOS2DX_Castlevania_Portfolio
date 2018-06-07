#pragma once

#include "Room.h"
#include "Object.h"

class CStageMap :public Node
{
private:
	StageData m_data;
	std::vector<CRoom*> m_Rooms;
	std::vector<CObject*> m_ObjectList;

	int m_nStageIdx;
	int m_nCurRIdx;
	int m_nLastRIdx;
public:
	virtual bool init(unsigned int nIdx);

	static CStageMap* create(unsigned int idx);
public:
	CRoom * getCurrentRoom();
	void setCurrentRoomIdx(int nIdx);
	int getCurrentRoomIdx();
	int getLastRoomIdx();
	void update(float fDelta);
public:
	void addObjectFromList(CObject* pObject);
	void removeObjectFromList(CObject* pObject);
private:
	void initSpace();
	void createRoom();
	void generateRooms();

	std::vector<CObject*>& getObjectList();
private:
	void createObstacle(CRoom* pRoom,int nIdx);
	void createEnemy(CRoom* pRoom,int nIdx);
	void recaculateObjectsZOrder(float fDelta);
	bool roomTileCheck(CRoom* pRoom,int &nX,int &nY,int &nRegX,int &nRegY);
};
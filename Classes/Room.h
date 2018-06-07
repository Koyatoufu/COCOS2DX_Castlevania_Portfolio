#pragma once

#include "Unit.h"

class CRoom :public Node 
{
private:
	Sprite *m_pFloor;
	bool m_isHere;
	bool m_isGone;

	Vec2 m_startPos;
	Rect m_endRect;
	Rect m_SpawnPoint;
	Node *m_pBg;
	TileData m_tileData;
	int m_nEnemyCount;
private:
	std::vector<CObject*> m_curObjects;
private:
	CRoom();
	virtual ~CRoom();
public:
	virtual bool init();

	CREATE_FUNC(CRoom);
public:
	void initRoom(std::string &FloorName,Node *pBg=nullptr);
	void pushObjects(CObject* pObject);

	void resetRoom();
	void clearRoom();
private:
	void initfloorData();
public:
	Vec2 getStartPos() const;
	Rect getEndRect() const;
	Rect getFloorBoundRect() const;
	Rect getSpwanPoints() const;

	bool getIsClear() const;
	bool getIsHere() const;
	bool getIsGone() const;
	std::vector<CObject*>& getObjectsList();
	TileReg getTileReg(int nX, int nY) const;
	void setTileReg(int nX, int nY,TileReg regData);
public:
	void addEnemyCount();
	void minEnemyCount();
	void removeObjectFromList(CObject* pObject);
private:
	void setIsHere(bool isHere);
	void setIsGone(bool isGone);
};
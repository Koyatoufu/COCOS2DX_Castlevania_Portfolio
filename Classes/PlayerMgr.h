#pragma once

#include "Player.h"

class CPlayerMgr
{
private:
	static CPlayerMgr* m_pInst;

	CPlayer *m_pCurPL;
	CPlayer *m_pResPL;

	bool m_isGameOver;
	bool m_isInit;
private:
	CPlayerMgr();
	~CPlayerMgr();
public:
	static CPlayerMgr* getInst();
	static void releaseInst();

	bool initialize();
	bool isPlayersAllDead();
	void changePlayer();

	void deadAccept(CPlayer* pPlayer);
public:
	CPlayer * getCurrentPlayer()
	{
		return m_pCurPL;
	};
	CPlayer * getRestorePlayer()
	{
		return m_pResPL;
	}
};


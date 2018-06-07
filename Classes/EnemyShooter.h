#pragma once

#include "Enemy.h"

class CEnemyShooter :public CEnemy
{
protected:
	CEnemyShooter();
	virtual ~CEnemyShooter();
public:
	virtual bool init(unsigned int nIdx);

	static CEnemyShooter* create(unsigned int idx);
	
	virtual void execute(float fDelta);
	virtual void setState(State idx);
};
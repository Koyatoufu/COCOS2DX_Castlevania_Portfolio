#pragma once

#include "Enemy.h"

class CEnemyRunner :public CEnemy
{
protected:
	CEnemyRunner();
	virtual ~CEnemyRunner();
public:
	virtual bool init(unsigned int nIdx);

	static CEnemyRunner* create(unsigned int idx);

	virtual void execute(float fDelta);
	virtual void setState(State idx);
};
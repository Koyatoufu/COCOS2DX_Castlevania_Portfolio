#pragma once

#include "Includes.h"

class CSoundMgr
{
private:
	static CSoundMgr * m_pInst;
	int m_nBgmID;
	int m_nEffectID;

	float m_fBgmVolume;
	float m_fEffectVolume;
private:
	CSoundMgr();
	CSoundMgr(const CSoundMgr&);
	~CSoundMgr();
public:
	static CSoundMgr* getInst();
	static void releaseInst();
public:
	void playBgm(std::string szFilePath,bool isLoop);
	void playEffect(std::string szFilePath);

	float getBgmVolume()const;
	float getEffectVolume()const;

	void setBgmVolume(float volume);
	void setEffectsVolume(float volume);
};
#include "SoundMgr.h"
#include "AudioEngine.h"

using namespace experimental;

CSoundMgr * CSoundMgr::m_pInst = nullptr;

CSoundMgr::CSoundMgr():m_nBgmID(-1),m_nEffectID(-1),m_fBgmVolume(1.0f),m_fEffectVolume(1.0f)
{
	m_fBgmVolume = UserDefault::getInstance()->getFloatForKey("bgm_volume");
	m_fEffectVolume = UserDefault::getInstance()->getFloatForKey("effect_volume");

	if (m_fBgmVolume == 0.0f)
		m_fBgmVolume = 1.0f;
	if (m_fEffectVolume == 0.0f)
		m_fEffectVolume = 1.0f;
}

CSoundMgr::CSoundMgr(const CSoundMgr &)
{
	
}

CSoundMgr::~CSoundMgr()
{
	UserDefault::getInstance()->setFloatForKey("bgm_volume", m_fBgmVolume);
	UserDefault::getInstance()->setFloatForKey("effect_volume", m_fEffectVolume);
	//AudioEngine::end();
}

CSoundMgr * CSoundMgr::getInst()
{
	if (!m_pInst)
	{
		m_pInst = new CSoundMgr();
	}
	return m_pInst;
}

void CSoundMgr::releaseInst()
{
	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}

void CSoundMgr::playBgm(std::string szFilePath, bool isLoop)
{
	if (m_nBgmID == -1)
	{
		m_nBgmID = AudioEngine::play2d(szFilePath, isLoop, m_fBgmVolume);
	}
	else
	{
		AudioEngine::stop(m_nBgmID);
		m_nBgmID = AudioEngine::play2d(szFilePath, isLoop, m_fBgmVolume);
	}
}

void CSoundMgr::playEffect(std::string szFilePath)
{
	m_nEffectID = AudioEngine::play2d(szFilePath, false, m_fEffectVolume);
}

float CSoundMgr::getBgmVolume() const
{
	return m_fBgmVolume;
}

float CSoundMgr::getEffectVolume() const
{
	return m_fEffectVolume;
}

void CSoundMgr::setBgmVolume(float volume)
{
	if (volume > 1.0f)
	{
		volume = 1.0f;
	}
	else if (volume < 0.0f)
	{
		volume = 0.0f;
	}

	m_fBgmVolume = volume;

	if(m_nBgmID!=-1)
		experimental::AudioEngine::setVolume(m_nBgmID, volume);
}

void CSoundMgr::setEffectsVolume(float volume)
{
	if (volume > 1.0f)
	{
		volume = 1.0f;
	}
	else if (volume < 0.0f)
	{
		volume = 0.0f;
	}

	m_fEffectVolume = volume;

	if(m_nEffectID = -1)
		experimental::AudioEngine::setVolume(m_nEffectID,volume);
}

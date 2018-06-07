#include "UserData.h"

#include "json\rapidjson.h"
#include "json\document.h"
#include "json\stringbuffer.h"
#include "json\prettywriter.h"

using namespace rapidjson;

CUserData::CUserData():m_nID(0),m_szName(),m_userPLIdx1(PLUnitIdx::NONE),m_userPLIdx2(PLUnitIdx::NONE),m_isCharsUnlock(),m_isStagesUnlock()
{
	loadData();
}

CUserData::~CUserData()
{
	saveData();
}

void CUserData::saveData()
{
	auto pUtil = FileUtils::getInstance();

	bool bResult = pUtil->writeStringToFile(createJson(), "data/userdata.json");

	CC_ASSERT(bResult);
}

void CUserData::loadData()
{
	auto pUtil = FileUtils::getInstance();

	std::string json = pUtil->getStringFromFile("data/userdata.json");

	CC_ASSERT(parseJson(json));
}

void CUserData::initUserData(std::string & Name)
{
	if (!m_szName.empty())
		return;
	
	srand(time(nullptr));
	m_nID = rand() % 123456;

	m_szName = Name;

	m_userPLIdx1 = PLUnitIdx::TREVOR;
	m_userPLIdx2 = PLUnitIdx::NONE;

	m_isCharsUnlock[0] = true;
	m_isStagesUnlock[0] = true;
}

bool CUserData::parseJson(std::string &json)
{
	if (json.empty())
		return false;

	Document document;
	document.Parse<0>(json.c_str());

	if (document.HasParseError())
		return false;

	if (document.HasMember("id") && document["id"].IsInt())
		m_nID = document["id"].GetInt();

	if (document.HasMember("name") && document["name"].IsString())
		m_szName = document["name"].GetString();

	if (document.HasMember("PlIdx1") && document["PlIdx1"].IsInt())
	{
		int nIdx = (document["PlIdx1"].GetInt());
		m_userPLIdx1 = (PLUnitIdx)nIdx;
	}

	if (document.HasMember("PlIdx2") && document["PlIdx2"].IsInt())
		m_userPLIdx2 = (PLUnitIdx)(document["PlIdx2"].GetInt());

	if (document.IsObject())
	{
		if (document.HasMember("charUnlock") && document["charUnlock"].IsArray())
		{
			const rapidjson::Value& arr = document["charUnlock"].GetArray();

			m_isCharsUnlock.reserve(arr.Capacity());
			m_isCharsUnlock.resize(arr.Size());
			for (unsigned int i = 0; i < arr.Size(); i++)
			{
				m_isCharsUnlock[i] = arr[i].GetBool();
			}
		}
	}

	if (document.IsObject())
	{
		if (document.HasMember("stageUnlock") && document["stageUnlock"].IsArray())
		{
			const rapidjson::Value& arr = document["stageUnlock"].GetArray();

			m_isStagesUnlock.reserve(arr.Capacity());
			m_isStagesUnlock.resize(arr.Size());
			for (unsigned int i = 0; i < arr.Size(); i++)
			{
				m_isStagesUnlock[i] = arr[i].GetBool();
			}
		}
	}

	return true;
}

std::string CUserData::createJson()
{
	Document document;

	document.SetObject();
	Document::AllocatorType &allocator = document.GetAllocator();

	rapidjson::Value id(rapidjson::kNumberType);
	id.SetInt(m_nID);
	document.AddMember("id", id, allocator);

	rapidjson::Value name(rapidjson::kStringType);
	name.SetString(m_szName.c_str(),m_szName.length(),allocator);
	document.AddMember("name", name, allocator);

	rapidjson::Value plIdx1(rapidjson::kNumberType);
	plIdx1.SetInt((int)m_userPLIdx1);
	document.AddMember("PlIdx1", plIdx1, allocator);

	rapidjson::Value plIdx2(rapidjson::kNumberType);
	plIdx2.SetInt((int)m_userPLIdx2);
	document.AddMember("PlIdx2", plIdx2, allocator);

	//charUnlock

	rapidjson::Value vecArr(rapidjson::kArrayType);

	for (auto item : m_isCharsUnlock)
	{
		bool is = item;

		rapidjson::Value val(rapidjson::kFalseType);
		val.SetBool(is);

		vecArr.PushBack(val, allocator);
	}

	document.AddMember("charUnlock", vecArr, allocator);

	//stageUnlock
	
	rapidjson::Value vecArr2(rapidjson::kArrayType);

	for (auto item : m_isStagesUnlock)
	{
		bool is = item;

		rapidjson::Value val(rapidjson::kFalseType);
		val.SetBool(is);

		vecArr2.PushBack(val, allocator);
	}

	document.AddMember("stageUnlock", vecArr2, allocator);


	StringBuffer buffer;

	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	std::string str(buffer.GetString());

	return str;
}


int CUserData::getId()
{
	return m_nID;
}

const std::string & CUserData::getName() const
{
	return m_szName;
}

PLUnitIdx CUserData::getUserPLIDX1()
{
	return m_userPLIdx1;
}

PLUnitIdx CUserData::getUserPLIDX2()
{
	return m_userPLIdx2;
}

const std::vector<bool> &CUserData::getCurGetChars() const
{
	return m_isCharsUnlock;
}

const std::vector<bool> &CUserData::getCurUnlockStages() const
{
	return m_isStagesUnlock;
}

void CUserData::setUserPLIDXs(PLUnitIdx idx1, PLUnitIdx idx2)
{
	m_userPLIdx1 = idx1;
	m_userPLIdx2 = idx2;
}

void CUserData::setIsGetChar(bool isGet,int nIdx)
{
	if (nIdx >= (int)m_isCharsUnlock.size() || nIdx<0)
		return;

	m_isCharsUnlock[nIdx] = isGet;
}

void CUserData::setIsUnlockStage(bool isClear, int nIdx)
{
	if (nIdx>=(int)m_isStagesUnlock.size() || nIdx<0)
		return;

	m_isStagesUnlock[nIdx] = isClear;
}

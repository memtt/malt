/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Options.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2019
*    AUTHOR   : Sébastien Valat (CERN) - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_OPTIONS_META_HPP
#define MALT_OPTIONS_META_HPP

/**********************************************************/
//std
#include <string>
#include <map>
#include <cassert>
#include <sstream>
#include <functional>
#include <json/JsonState.h>

/**********************************************************/
//iniparser
extern "C" {
	#include <iniparser.h>
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
/*
 * Provide some helper functions to use INI parser.
**/
struct IniParserHelper
{
	static std::string extractSectionName ( const char * key );
	static void setEntry (dictionary * dic, const char * key, const char* value );
	static void setEntry (dictionary * dic, const char * key, bool value);
	static void setEntry (dictionary * dic, const char * key, int value);
	template <class T> static void setEntry(dictionary * dic, const char * key, const T & value);
};

/**********************************************************/
template <class T> void IniParserHelper::setEntry(dictionary * dic, const char * key, const T & value)
{
	std::stringstream out;
	out << value;
	setEntry(dic, key, out.str().c_str());
}

/**********************************************************/
class OptionMetaBase
{
	public:
		OptionMetaBase(const std::string & group, const std::string & key);
		virtual ~OptionMetaBase(void) = 0;
		OptionMetaBase & setDoc(const std::string & value);
		const std::string & getGroup(void) const;
		const std::string & getKey(void) const;
		const std::string & getDoc(void) const;
	 	virtual std::string toString(void) const = 0;
		std::string getGroupKey(void) const;
		virtual bool operator!=(const OptionMetaBase & value) const = 0;
		virtual void loadFromIni(dictionary* iniDic) = 0;
		virtual void convertToJson(htopml::JsonState & json) const = 0;
		virtual void saveToIni(dictionary* iniDic) const = 0;
	private:
		std::string group;
		std::string key;
		std::string doc;
};

/**********************************************************/
template <class T>
class OptionMeta : public OptionMetaBase
{
	public:
		OptionMeta(const std::string & group, const std::string & key, T & value);
		bool operator!=(const OptionMetaBase & value) const override;
		void loadFromIni(dictionary* iniDic) override;
		void convertToJson(htopml::JsonState & json) const override;
		void saveToIni(dictionary* iniDic) const override;
		std::string toString(void) const override;
		const T & getValue(void) const;
	private:
		T & value;
};

/**********************************************************/
template<>
inline void OptionMeta<std::string>::loadFromIni(dictionary* iniDic)
{
	char key[4096];
	snprintf(key, sizeof(key), "%s:%s", this->getGroup().c_str(), this->getKey().c_str());
	this->value = iniparser_getstring(iniDic,key,(char*)this->value.c_str());
}

/**********************************************************/
template<>
inline void OptionMeta<bool>::loadFromIni(dictionary* iniDic)
{
	char key[4096];
	snprintf(key, sizeof(key), "%s:%s", this->getGroup().c_str(), this->getKey().c_str());
	this->value = iniparser_getboolean(iniDic,key,this->value);
}

/**********************************************************/
template <class T> T & setByString(T & element, const std::string & value)
{
	element = value;
	return element;
}

/**********************************************************/
template <class T>
OptionMeta<T>::OptionMeta(const std::string & group, const std::string & key, T & value)
	:OptionMetaBase(group, key)
	,value(value)
{
}

/**********************************************************/
template <class T>
bool OptionMeta<T>::operator!=(const OptionMetaBase & value) const
{
	const OptionMeta<T> * casted = dynamic_cast< const OptionMeta<T> * >(&value);
	if (casted == nullptr)
		return true;
	else
		return this->value != casted->value;
}

/**********************************************************/
template <class T>
void OptionMeta<T>::loadFromIni(dictionary* iniDic)
{
	setByString(this->value, iniparser_getstring(iniDic,this->getGroupKey().c_str(),(char*)this->toString().c_str()));
}

/**********************************************************/
template <class T>
void OptionMeta<T>::convertToJson(htopml::JsonState & json) const
{
	json.printField(this->getKey().c_str(),this->value);
}

/**********************************************************/
template <class T>
void OptionMeta<T>::saveToIni(dictionary* iniDic) const
{
	IniParserHelper::setEntry(iniDic,this->getGroupKey().c_str(),this->value);
}

/**********************************************************/
template <class T>
std::string OptionMeta<T>::toString(void) const
{
	std::stringstream out;
	out << this->value;
	return out.str();
}

/**********************************************************/
template <class T>
const T & OptionMeta<T>::getValue(void) const
{
	return this->value;
}

}

#endif //MALT_OPTIONS_META_HPP

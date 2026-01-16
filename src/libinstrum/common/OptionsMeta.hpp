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
#include <functional>

/**********************************************************/
//iniparser
extern "C" {
	#include <iniparser.h>
}

/**********************************************************/
namespace htopml {
	class JsonState;
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
class OptionMetaBase
{
	public:
		OptionMetaBase(const std::string & group, const std::string & key);
		virtual ~OptionMetaBase(void) = 0;
		OptionMetaBase & setDoc(const std::string & value);
		OptionMetaBase & setDefault(const std::string & value);
		const std::string & getGroup(void) const;
		const std::string & getKey(void) const;
		const std::string & getDoc(void) const;
		const std::string & getDefaultValue(void) const;
		const std::string & getGroupKey(void) const;
		virtual void assignDefault(void) = 0;
		virtual bool operator!=(const OptionMetaBase & value) const = 0;
		virtual void loadFromIni(dictionary* iniDic) = 0;
		virtual void convertToJson(htopml::JsonState & json) const = 0;
		virtual void saveToIni(dictionary* iniDic) const = 0;
	private:
		std::string group;
		std::string key;
		std::string doc;
		std::string defaultValue;
};

/**********************************************************/
template <class T>
class OptionMeta : public OptionMetaBase
{
	public:
		OptionMeta<T>(const std::string & group, const std::string & key, T & value);
		OptionMeta<T> & setConverter(std::function<std::string(const T&)> toString, std::function<std::string(const T&)> fromString);
		void assignDefault(void) override;
		bool operator!=(const OptionMetaBase & value) const override;
		void loadFromIni(dictionary* iniDic) override;
		void convertToJson(htopml::JsonState & json) const override;
		void saveToIni(dictionary* iniDic) const override;
	private:
		T & value;
		std::function<std::string(const T&)> toString;
		std::function<T(const std::string &)> fromString;
};

/**********************************************************/
template <class T>
OptionMeta<T>::OptionMeta(const std::string & group, const std::string & key, T & value)
	:OptionMetaBase(group, key)
	,value(value)
{
}

/**********************************************************/
template <class T>
OptionMeta<T> & OptionMeta<T>::setConverter(std::function<std::string(const T&)> toString, std::function<std::string(const T&)> fromString)
{
	this->toString = toString;
	this->fromString = fromString;
}

/**********************************************************/
template <class T>
void OptionMeta<T>::assignDefault(void)
{
	this->value = this->fromString(this->defaultValue);
}

/**********************************************************/
template <class T>
bool OptionMeta<T>::operator!=(const OptionMetaBase & value) const
{
	return this->value != value.value;
}

/**********************************************************/
template <class T>
void OptionMeta<T>::loadFromIni(dictionary* iniDic)
{
	const std::string tmp = this->toString(this->value);
	this->value = this->formString(iniparser_getstring(iniDic,this->getGroupKey(),(char*)tmp.c_str()));
}

/**********************************************************/
template <class T>
void OptionMeta<T>::convertToJson(htopml::JsonState & json) const
{
	json.printField(this->getKey(),this->value);
}

/**********************************************************/
template <class T>
void OptionMeta<T>::saveToIni(dictionary* iniDic) const
{
	IniParserHelper::setEntry(iniDic,this->getGroupKey(),this->value);
}

}

#endif //MALT_OPTIONS_META_HPP

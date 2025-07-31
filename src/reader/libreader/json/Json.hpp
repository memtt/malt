/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/json/Json.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_JSON_JSON_HPP
#define MALT_JSON_JSON_HPP

/**********************************************************/
#include <ostream>
#include <vector>
#include <list>
#include <map>
#include "JsonIterator.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
class JsonItems
{
	public:
		typedef JsonIterator iterator;
		typedef JsonIterator const_iterator;
	public:
		JsonItems(const JsonNode * node) {this->node = node;};
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
	private:
		bool is_iterable(void) const;
	protected:
		const JsonNode * node;
};

/**********************************************************/
class Json
{
	public:
		typedef JsonArrayIterator iterator;
		typedef JsonArrayIterator const_iterator;
	public:
		Json(const JsonNode * node);
		virtual ~Json(void);
		Json at(const std::string & key) const;
		Json at(size_t id) const;
		bool contains(const std::string & key) const;
		Json operator[](const std::string & key) const;
		Json operator[](size_t id) const;
		void get_to(std::string & value) const;
		void get_to(size_t & value) const;
		void get_to(ssize_t & value) const;
		void get_to(bool & value) const;
		void get_to(int & value) const;
		void get_to(unsigned long long & value) const;
		template <class T> void get_to(T & value) const;
		size_t size(void) const;
		bool is_array(void) const;
		bool is_object(void) const;
		bool is_string(void) const;
		bool is_bool(void) const;
		bool is_iterable(void) const;
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		template <class T> bool operator==(const T & value) const;
		JsonItems items(void) const {return JsonItems(this->node);};
	public:
		friend std::ostream & operator<<(std::ostream & out, const Json & json);
	protected:
		const JsonNode * node;
};

/**********************************************************/
class JsonRoot : public Json
{
	public:
		JsonRoot(JsonNode * node);
		JsonRoot(JsonRoot & node) = delete;
		JsonRoot(JsonRoot && node);
		virtual ~JsonRoot(void);
};

/**********************************************************/
JsonRoot parse(const std::string & value, const std::string & filename = "MEMORY");
JsonRoot parse(const char * value, size_t size, const std::string & filename = "MEMORY");

/**********************************************************/
template <class T>
bool Json::operator==(const T & value) const
{
	T cur;
	this->get_to(cur);
	return cur == value;
}

/**********************************************************/
template <class T>
void Json::get_to(T & value) const
{
	from_json(*this, value);
}

/**********************************************************/
template<class T>
void from_json(const Json & json, std::vector<T> & value)
{
	assert(json.is_array());
	value.clear();
	value.reserve(json.size());
	for (auto it : json) {
		value.emplace_back();
		it.get_to(value.back());
	}
}

/**********************************************************/
template<class T, class U>
void from_json(const Json & json, std::map<T, U> & value)
{
	assert(json.is_object());
	value.clear();
	for (auto it : json.items()) {
		it.value().get_to(value[it.key()]);
	}
}

}

#endif //MALT_JSON_JSON_HPP

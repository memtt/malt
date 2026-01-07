/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/Json.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <iostream>
#include "Parser.hpp"
#include "Json.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
Json::Json(const JsonNode * node)
{
	this->node = node;
}

/**********************************************************/
Json::~Json(void)
{
	//nothing
}

/**********************************************************/
Json Json::at(const std::string & key) const
{
	//error
	if (this->node ==nullptr || node->type != JSON_NODE_OBJECT)
		throw JsonException("Using at(std::string) with a non object type !");

	//search key
	for (auto it : this->items()) {
		if (it.key() == key)
			return it.value();
	}

	//null
	return Json(nullptr);
}

/**********************************************************/
Json Json::at(size_t id) const
{
	//error
	if (this->node ==nullptr || node->type != JSON_NODE_ARRAY)
		throw JsonException("Using at(size_t) with a non array type !");

	//search key
	if (id < this->node->view.vector->size())
		return Json(&((*this->node->view.vector)[id]));
	else
		return Json(nullptr);
}

/**********************************************************/
bool Json::contains(const std::string & key) const
{
	//error
	if (this->node ==nullptr || node->type != JSON_NODE_OBJECT)
		throw JsonException("Using at(std::string) with a non object type !");

	//search key
	for (auto it : this->items()) {
		if (it.key() == key)
			return true;
	}

	//null
	return false;
}

/**********************************************************/
Json Json::operator[](const std::string & key) const
{
	return this->at(key);
}

/**********************************************************/
Json Json::operator[](size_t id) const
{
	return this->at(id);
}

/**********************************************************/
void Json::get_to(std::string & value) const
{
	//error
	if (this->node == nullptr || this->node->type != JSON_NODE_STRING)
		throw JsonException("Try to convert a node which is not a string into a string !");

	//do it
	value = this->node->value.toString();
}

/**********************************************************/
void Json::get_to(size_t & value) const
{
	//error
	if (this->node == nullptr || this->node->type != JSON_NODE_INTEGER)
		throw JsonException("Try to convert a node which is not an integer into an integer !");

	//do it
	std::string str = this->node->value.toString();
	ssize_t status = sscanf(str.c_str(), "%zu", &value);

	//error
	if (status != 1)
		throw JsonException("Failed to convert the number into size_t !");
}

/**********************************************************/
void Json::get_to(ssize_t & value) const
{
	//error
	if (this->node == nullptr || this->node->type != JSON_NODE_INTEGER)
		throw JsonException("Try to convert a node which is not an integer into an integer !");

	//do it
	std::string str = this->node->value.toString();
	ssize_t status = sscanf(str.c_str(), "%zd", &value);

	//error
	if (status != 1)
		throw JsonException("Failed to convert the number into ssize_t !");
}

/**********************************************************/
void Json::get_to(unsigned long long & value) const
{
	//error
	if (this->node == nullptr || this->node->type != JSON_NODE_INTEGER)
		throw JsonException("Try to convert a node which is not an integer into an integer !");

	//do it
	std::string str = this->node->value.toString();
	ssize_t status = sscanf(str.c_str(), "%llu", &value);

	//error
	if (status != 1)
		throw JsonException("Failed to convert the number into size_t !");
}

/**********************************************************/
void Json::get_to(bool & value) const
{
	//error
	if (this->node == nullptr || this->node->type != JSON_NODE_BOOL)
		throw JsonException("Try to convert a node which is not a boolean into a bool !");

	//do it
	std::string str = this->node->value.toString();
	if (str == "true")
		value = true;
	else if (str == "false")
		value = false;
	else
		throw JsonException("Invalid value for a boolean, should be (true|false) !");
}

/**********************************************************/
void Json::get_to(int & value) const
{
	//error
	if (this->node == nullptr || this->node->type != JSON_NODE_INTEGER)
		throw JsonException("Try to convert a node which is not an integer into an integer !");

	//do it
	std::string str = this->node->value.toString();
	ssize_t v;
	ssize_t status = sscanf(str.c_str(), "%zd", &v);
	value = v;

	//error
	if (status != 1)
		throw JsonException("Failed to convert the number into size_t !");
	if (value != v)
		throw JsonException("Overflow while converting the number into int !");
}

/**********************************************************/
size_t Json::size(void) const
{
	//error
	if (this->is_iterable() == false)
		throw JsonException("Try to use size() on a node which is not an array or object !");

	//select
	if (this->node->type == JSON_NODE_ARRAY)
		return this->node->view.vector->size();
	else if (this->node->type == JSON_NODE_OBJECT)
		return this->node->view.vmap->size();
	else
		throw JsonException("Unsupported mode !");
}

/**********************************************************/
bool Json::is_array(void) const
{
	return (this->node != nullptr && this->node->type == JSON_NODE_ARRAY);
}

/**********************************************************/
bool Json::is_object(void) const
{
	return (this->node != nullptr && this->node->type == JSON_NODE_OBJECT);
}

/**********************************************************/
bool Json::is_string(void) const
{
	return (this->node != nullptr && this->node->type == JSON_NODE_STRING);
}

/**********************************************************/
bool Json::is_bool(void) const
{
	return (this->node != nullptr && this->node->type == JSON_NODE_BOOL);
}

/**********************************************************/
bool Json::is_iterable(void) const
{
	return (this->node != nullptr && (this->node->type == JSON_NODE_ARRAY || this->node->type == JSON_NODE_OBJECT));
}

/**********************************************************/
bool JsonItems::is_iterable(void) const
{
	return (this->node != nullptr && (this->node->type == JSON_NODE_ARRAY || this->node->type == JSON_NODE_OBJECT));
}

/**********************************************************/
Json::iterator Json::begin()
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return Json::iterator(this->node, 0);
}

/**********************************************************/
Json::iterator Json::end()
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return Json::iterator(this->node, this->size());
}

/**********************************************************/
Json::const_iterator Json::begin() const
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return Json::const_iterator(this->node, 0);
}

/**********************************************************/
Json::const_iterator Json::end() const
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return Json::const_iterator(this->node, this->size());
}

/**********************************************************/
JsonItems::iterator JsonItems::begin()
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return JsonItems::iterator(this->node, 0);
}

/**********************************************************/
JsonItems::iterator JsonItems::end()
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return JsonItems::iterator(this->node, Json(this->node).size());
}

/**********************************************************/
JsonItems::const_iterator JsonItems::begin() const
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return JsonItems::const_iterator(this->node, 0);
}

/**********************************************************/
JsonItems::const_iterator JsonItems::end() const
{
	if (this->is_iterable() == false)
		throw JsonException("Try to iterate on a node which is not iterable !");
	return JsonItems::const_iterator(this->node, Json(this->node).size());
}

/**********************************************************/
std::ostream & operator<<(std::ostream & out, const Json & json)
{
	if (json.is_array()) {
		out << "[";
		for (const auto & it : json.items()) {
			out << it.value() << ",";
		}
		out << "]";
	} else if (json.is_object()) {
		out << "{";
		for (const auto & it : json.items()) {
			out << '"' << it.key() << '"' << ":" << it.value() << ",";
		}
		out << "}";
	} else if (json.is_string()) {
		std::string tmp;
		json.get_to(tmp);
		out << '"' << tmp << '"';
	} else {
		out << json.node->value.toString();
	}
	return out;
}

/**********************************************************/
JsonRoot parse(const char * value, size_t size, const std::string & filename)
{
	//build
	JsonString str;
	str.value = value;
	str.len = size;

	//build cursor
	JsonFileCursor cursor;
	cursor.fname = &filename;
	cursor.data = &str;
	
	//get next
	JsonNode * root = new JsonNode();
	JsonCharType next = cursor.forwardNextStart();
	switch (next) {
		case JSON_CHAR_ARRAY_START:
		case JSON_CHAR_OBJECT_START:
			cursor.parseChild(*root);
			break;
		default:
			cursor.raiseError("Invalid start, should be object or array !");
			break;
	}

	//ok
	return JsonRoot(root);
}

/**********************************************************/
JsonRoot::JsonRoot(JsonNode * node)
	:Json(node)
{
	assert(node != nullptr);
	this->node = node;
}

/**********************************************************/
JsonRoot::JsonRoot(JsonRoot && node)
	:Json(node)
{
	node.node = nullptr;
}

/**********************************************************/
JsonRoot::~JsonRoot(void)
{
	if (this->node != nullptr) {
		delete this->node;
		this->node = nullptr;
	}
}

/**********************************************************/
JsonRoot parse(const std::string & value, const std::string & filename)
{
	return parse(value.c_str(), value.size(), filename);
}

}
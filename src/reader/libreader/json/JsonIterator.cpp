/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/JsonIterator.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include "Exception.hpp"
#include "JsonIterator.hpp"
#include "Json.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
JsonIteratorValue::JsonIteratorValue(const JsonNode * node, size_t cursor)
{
	assert(node != nullptr);
	this->node = node;
	this->cursor = cursor;
}

/**********************************************************/
size_t JsonIteratorValue::index(void) const
{
	return this->cursor;
}

/**********************************************************/
std::string JsonIteratorValue::key(void) const
{
	if (this->node->type != JSON_NODE_OBJECT)
		throw JsonException("Try to get key for an iterable which is not an object !");
	return this->node->view.vmap->at(this->cursor).first.toString();
}

/**********************************************************/
Json JsonIteratorValue::value(void) const
{
	if (this->node->type == JSON_NODE_OBJECT)
		return Json(&(this->node->view.vmap->at(this->cursor).second));
	else if (this->node->type == JSON_NODE_ARRAY)
		return Json(&(this->node->view.vector->at(this->cursor)));
	else
		throw JsonException("Unknown type !");

}

/**********************************************************/
Json JsonIteratorValue::operator*(void) const
{
	if (this->node->type == JSON_NODE_OBJECT)
		return Json(&(this->node->view.vmap->at(this->cursor).second));
	else if (this->node->type == JSON_NODE_ARRAY)
		return Json(&(this->node->view.vector->at(this->cursor)));
	else
		throw JsonException("Unknown type !");
}

/**********************************************************/
JsonIterator::JsonIterator(const JsonNode * node, size_t cursor)
{
	this->node = node;
	this->cursor = cursor;
}

/**********************************************************/
JsonIterator & JsonIterator::operator++(void)
{
	if (this->cursor < Json(this->node).size())
		this->cursor++;
	return *this;
}

/**********************************************************/
bool operator<(const JsonIterator & a, const JsonIterator & b)
{
	return a.cursor < b.cursor && a.node == b.node;
}

/**********************************************************/
bool operator!=(const JsonIterator & a, const JsonIterator & b)
{
	return a.cursor != b.cursor || a.node != b.node;
}

/**********************************************************/
bool operator-(const JsonIterator & a, const JsonIterator & b)
{
	return a.cursor - b.cursor;
}

/**********************************************************/
JsonIteratorValue JsonIterator::operator*(void) const
{
	return JsonIteratorValue(this->node, this->cursor);
}

/**********************************************************/
JsonArrayIterator::JsonArrayIterator(const JsonNode * node, size_t cursor)
{
	this->node = node;
	this->cursor = cursor;
}

/**********************************************************/
JsonArrayIterator & JsonArrayIterator::operator++(void)
{
	if (this->cursor < Json(this->node).size())
		this->cursor++;
	return *this;
}

/**********************************************************/
bool operator<(const JsonArrayIterator & a, const JsonArrayIterator & b)
{
	return a.cursor < b.cursor && a.node == b.node;
}

/**********************************************************/
bool operator!=(const JsonArrayIterator & a, const JsonArrayIterator & b)
{
	return a.cursor != b.cursor || a.node != b.node;
}

/**********************************************************/
bool operator-(const JsonArrayIterator & a, const JsonArrayIterator & b)
{
	return a.cursor - b.cursor;
}

/**********************************************************/
Json JsonArrayIterator::operator*(void) const
{
	if (this->node->type == JSON_NODE_OBJECT)
		return Json(&(this->node->view.vmap->at(this->cursor).second));
	else if (this->node->type == JSON_NODE_ARRAY)
		return Json(&(this->node->view.vector->at(this->cursor)));
	else
		throw JsonException("Unknown type !");
}

}

#ifndef MALT_JSON_JSON_ITERATOR_HPP
#define MALT_JSON_JSON_ITERATOR_HPP

#include "Node.hpp"

namespace MALTJson
{

class Json;

class JsonIteratorValue
{
	public:
		JsonIteratorValue(const JsonNode * node = nullptr, size_t cursor = 0);
		size_t index(void) const;
		std::string key(void) const;
		Json value(void) const;
		Json operator*(void) const;
	private:
		const JsonNode * node{nullptr};
		size_t cursor{0};
};

class JsonIterator
{
	public:
		JsonIterator(const JsonNode * node = nullptr, size_t cursor = 0);
		JsonIterator & operator++(void);
	public:
		friend bool operator<(const JsonIterator & a, const JsonIterator & b);
		friend bool operator!=(const JsonIterator & a, const JsonIterator & b);
		friend bool operator-(const JsonIterator & a, const JsonIterator & b);
		JsonIteratorValue operator*(void) const;
	private:
		const JsonNode * node{nullptr};
		size_t cursor{0};
};

class JsonArrayIterator
{
	public:
		JsonArrayIterator(const JsonNode * node = nullptr, size_t cursor = 0);
		JsonArrayIterator & operator++(void);
	public:
		friend bool operator<(const JsonArrayIterator & a, const JsonArrayIterator & b);
		friend bool operator!=(const JsonArrayIterator & a, const JsonArrayIterator & b);
		friend bool operator-(const JsonArrayIterator & a, const JsonArrayIterator & b);
		Json operator*(void) const;
	private:
		const JsonNode * node{nullptr};
		size_t cursor{0};
};

}

#endif //MALT_JSON_JSON_ITERATOR_HPP

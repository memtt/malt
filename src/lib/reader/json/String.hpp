#ifndef MALT_JSON_STRING_HPP
#define MALT_JSON_STRING_HPP

#include <cstdlib>
#include <string>

namespace MALTJson
{

struct JsonString
{
	JsonString(void);
	JsonString(const char * input);
	const char * value{nullptr};
	size_t len{0};
	std::string toString(void) const;
};

bool operator<(const JsonString & a, const JsonString & b);
bool operator==(const JsonString & a, const JsonString & b);

}

#endif //MALT_JSON_STRING_HPP

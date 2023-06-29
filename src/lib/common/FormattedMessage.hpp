/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_FORMALTED_MESSAGE_HPP
#define MALT_FORMALTED_MESSAGE_HPP

/********************  HEADERS  *********************/
//standard
#include <vector>
#include <string>
#include <sstream>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  TYPES  **********************/
/**
 * @brief Vector to store the values to replace into the formatted message. 
**/
typedef std::vector<std::string> FormattedMessageEntries;
	
/*********************  CLASS  **********************/
/**
 * Provide a short class to use formatted message with full C++ stream operators support
 * and avoid to manually manage local fixed size buffers to use sprintf. Similar usage
 * can be found into QT translation system, thanks to them for the idea.
 * 
 * @brief Short class to format messages with object support.
**/
class FormattedMessage
{
	public:
		FormattedMessage(const char * format);
		FormattedMessage(const std::string & format);
		virtual ~FormattedMessage(void);
		template <class T> FormattedMessage & arg(const T & value);
		FormattedMessage & arg(const std::string & value);
		FormattedMessage & arg(const char * value);
		FormattedMessage & argStrErrno(void);
		std::string toString(void) const;
		void toStream(std::ostream & out) const;
		virtual void end(void);
	public:
		friend std::ostream & operator << (std::ostream & out,const FormattedMessage & message);
	protected:
		void pushValue(std::ostream & out,int id) const;
	private:
		/** Store the string format **/
		std::string format;
		/** Store the entries to replace into format into toString() method. **/
		FormattedMessageEntries entries;
};

/*******************  FUNCTION  *********************/
/**
 * Setup argument value. First call will setup %1, second %2... All
 * values are converted to string by standard STL stream operator onto stringstream object.
 * @param value Value to convert to string and to setup as argument value.
**/
template <class T>
FormattedMessage& FormattedMessage::arg(const T& value)
{
	//convert
	std::stringstream buffer;
	buffer << value;
	
	//insert
	entries.push_back(buffer.str());
	
	return *this;
}

}

#endif //MALT_FORMALTED_MESSAGE_HPP

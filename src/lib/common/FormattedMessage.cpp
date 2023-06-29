/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cerrno>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
//internals
#include "FormattedMessage.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the class, only setup the local format value from the given one.
 * Values are defined by using syntaxe %1, %2, %3... To print %, just do like
 * printf by using %%.
 * @param format Define the format of the message. NULL not allowed.
**/
FormattedMessage::FormattedMessage(const char* format)
{
	assert(format != NULL);
	this->format = format;
}

/*******************  FUNCTION  *********************/
/**
 * Constructor of the class, only setup the local format value from the given one.
 * Values are defined by using syntaxe %1, %2, %3... To print %, just do like
 * printf by using %%.
 * @param format Define the format of the message.
**/
FormattedMessage::FormattedMessage(const std::string & format)
{
	this->format = format;
}

/*******************  FUNCTION  *********************/
/**
 * Nothing to do, only provided for inheritance requirement for virtuel destructor.
**/
FormattedMessage::~FormattedMessage(void)
{

}

/*******************  FUNCTION  *********************/
/**
 * Nothing to do, only support for inheritance usage. This function can be extended
 * to provided error reporting (throw exception or print and exit program)
**/
void FormattedMessage::end(void)
{
	
}

/*******************  FUNCTION  *********************/
/**
 * Apply the given values on format and flush into the given stream
**/
void FormattedMessage::toStream(std::ostream& out) const
{
	//local vars to avoid search them every time
	const char * cformat = format.c_str();
	size_t size = format.size();
	
	//loop over format
	size_t i = 0;
	while (i < size)
	{
		//has %, capture of %%
		if (format[i] == '%')
		{
			//foward
			i++;
			//ok capture
			if (format[i] != '%')
			{
				//get id
				int id = atoi(cformat+i);
				assert(id != 0);
				
				//push value
				pushValue(out,id);
				
				//move forward to skip %ID
				while (format[i] >= '0' && format[i] <= '9') {i++;};
			} else {
				//print and forward 2 steps to skip %%
				out << '%';
				i++;
			}
		} else {
			out << format[i++];
		}
	}
}

/*******************  FUNCTION  *********************/
/**
 * Apply the given values on format and return the final string.
**/
std::string FormattedMessage::toString(void) const
{
	//buffer to accumulate
	std::stringstream buffer;
	toStream(buffer);

	return buffer.str();
}

/*******************  FUNCTION  *********************/
/**
 * Push the appropriate value depending on ID or report missing ID and abort.
 * We consider numbering from 1, not 0 which is invalid.
 * @param out Stream used to write the output.
 * @param id Id of the value to print. Will print %ID if not found and print 
 * warning in error stream.
**/
void FormattedMessage::pushValue(std::ostream & out,int id) const
{
	//manage errors
	if (id == 0 || (size_t)id > entries.size())
	{
		fprintf(stderr,"Warning : missing ID=%d in formatted message '%s' !\n",id,format.c_str());
		out << '%' << id;
	} else {
		out << entries[id-1];
	}
}

/*******************  FUNCTION  *********************/
/**
 * Specific implementation to avoid useless intermediated copies.
**/
FormattedMessage& FormattedMessage::arg(const std::string& value)
{
	entries.push_back(value);
	return *this;
}

/*******************  FUNCTION  *********************/
/**
 * Specific implementation to avoid useless intermediated copies.
**/
FormattedMessage& FormattedMessage::arg(const char* value)
{
	if (value == NULL)
		entries.push_back("(null)");
	else
		entries.push_back(value);
	return *this;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const FormattedMessage& message)
{
	message.toStream(out);
	return out;
}

/*******************  FUNCTION  *********************/
FormattedMessage & FormattedMessage::argStrErrno(void)
{
	this->arg(strerror(errno));
	return *this;
}

}

/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 03/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_FAST_BUFFERED_STREAM_HPP
#define MALT_FAST_BUFFERED_STREAM_HPP

/********************  HEADERS  *********************/
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <cassert>
#include <cstring>

namespace htopml
{

/*********************  CLASS  **********************/
class FastBufferedStream
{
	public:
		FastBufferedStream(std::ostream * stream,size_t bufferSize = 4096);
		~FastBufferedStream(void);
		void flush(bool alsoFlushStream = false);
		size_t availSize(void) const;
	public:
		friend FastBufferedStream & operator << (FastBufferedStream & out,char value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,const char * value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,char * value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,const std::string & value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,int value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,long value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,unsigned long value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,unsigned long long value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,void * value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,float value);
		friend FastBufferedStream & operator << (FastBufferedStream & out,double value);
		template <class T> friend FastBufferedStream & operator << (FastBufferedStream & out,const T & value);
	private:
		char * buffer;
		size_t bufferSize;
		size_t position;
		std::ostream * stream;
};

/*******************  FUNCTION  *********************/
inline FastBufferedStream::FastBufferedStream(std::ostream* stream, size_t bufferSize)
{
	//check errors
	assert(stream != NULL);
	assert(bufferSize > 256);

	this->stream = stream;
	this->bufferSize = bufferSize;
	this->buffer = new char[bufferSize];
	this->position = 0;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream::~FastBufferedStream(void)
{
	flush();
	delete [] buffer;
}

/*******************  FUNCTION  *********************/
inline void FastBufferedStream::flush(bool alsoFlushStream)
{
	if (position > 0)
	{
		stream->write(buffer,position);
		if (alsoFlushStream)
			stream->flush();
		position = 0;
	}
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, char value)
{
	assert(out.position <= out.bufferSize);
	if (out.position >= out.bufferSize)
		out.flush();
	
	out.buffer[out.position++] = value;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, char * value)
{
	out << (const char*)value;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, const char * value)
{
	size_t position = out.position;
	size_t bufferSize = out.bufferSize;
	char * buffer = out.buffer;

	assert(position <= bufferSize);
	while (*value != '\0')
	{
		if (position >= bufferSize)
		{
			out.position = position;
			out.flush();
			position = out.position;
		}
		buffer[position++] = *value;
		value++;
	}
	
	out.position = position;	
	return out;
}

/*******************  FUNCTION  *********************/
inline size_t FastBufferedStream::availSize(void) const
{
	assert(bufferSize >= position);
	return bufferSize - position;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, const std::string& value)
{
	size_t availSize = out.availSize();
	
	if (availSize > value.size())
	{
		memcpy(out.buffer+out.position,value.c_str(),value.size());
		out.position += value.size();
	} else {
		out.flush();
		out.stream->write(value.c_str(),value.size());
	}
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, int value)
{
	assert(out.bufferSize >= 256);

	if (out.availSize() < 256)
		out.flush();
	int size = snprintf(out.buffer+out.position,out.availSize(),"%d",value);
	out.position += size;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, long value)
{
	assert(out.bufferSize >= 256);

	if (out.availSize() < 256)
		out.flush();
	int size = snprintf(out.buffer+out.position,out.availSize(),"%ld",value);
	out.position += size;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, unsigned long value)
{
	assert(out.bufferSize >= 256);

	if (out.availSize() < 256)
		out.flush();
	int size = snprintf(out.buffer+out.position,out.availSize(),"%lu",value);
	out.position += size;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, unsigned long long value)
{
	assert(out.bufferSize >= 256);

	if (out.availSize() < 256)
		out.flush();
	int size = snprintf(out.buffer+out.position,out.availSize(),"%llu",value);
	out.position += size;
	return out;
}


/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, float value)
{
	assert(out.bufferSize >= 256);

	if (out.availSize() < 256)
		out.flush();
	int size = snprintf(out.buffer+out.position,out.availSize(),"%g",value);
	out.position += size;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, double value)
{
	assert(out.bufferSize >= 256);

	if (out.availSize() < 256)
		out.flush();
	int size = snprintf(out.buffer+out.position,out.availSize(),"%lg",value);
	out.position += size;
	return out;
}

/*******************  FUNCTION  *********************/
inline FastBufferedStream& operator<<(FastBufferedStream& out, void * value)
{
	assert(out.bufferSize >= 32);

	if (out.availSize() < 32)
		out.flush();
	int size = 0;
	if (value == NULL)
		size = snprintf(out.buffer+out.position,out.availSize(),"0x0");
	else
		size = snprintf(out.buffer+out.position,out.availSize(),"%p",value);
	out.position += size;
	return out;
}

/*******************  FUNCTION  *********************/
template <class T>
inline FastBufferedStream& operator<<(FastBufferedStream& out, const T & value)
{
	out.flush();
	*out.stream << value;
	return out;
}

}

#endif //MALT_FAST_BUFFERED_STREAM_HPP

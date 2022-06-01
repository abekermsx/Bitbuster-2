// Copyright (c) 2003-2004 Eli-Jean Leyssens
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <fstream>
#include <iostream>
using std::ofstream;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::fstream;
using std::ios;
#include "filesnstreams.h"
using std::vector;

bool load_file( const char * path, vector<char> & data)
{
	bool	ret = true;
	
	ifstream	file;
	file.open( path, ios::in|ios::binary);
	if ( file.is_open())
	{
		file.seekg( 0, ios::end);
		size_t size = file.tellg();
		file.seekg( 0, ios::beg);
		if ( !file.fail())
		{
			data.resize( size);
			file.read( &data[0], size);
			if ( file.fail())
			{
				cerr << "Failed to read (all) bytes from file " << path << endl;
				ret = false;
			}
		}
		else
		{
			cerr << "Failed to determine length of file " << path << endl;
			ret = false;
		}
		file.close();
	}
	else
	{
		cerr << "Failed to open " << path << " for input" << endl;
		ret = false;
	}
	return ret;
}

bool save_file( const char * path, char * data, size_t size)
{
	bool	ret = true;
	
	ofstream	file;
	file.open( path, ios::in|ios::binary|ios::out|ios::trunc);
	if ( file.is_open())
	{
		file.write( data, size);
		if ( file.fail())
		{
			cerr << "Failed to write (all) bytes to file " << path << endl;
			ret = false;
		}
		file.close();
	}
	else
	{
		cerr << "Failed to open " << path << " for output" << endl;
		ret = false;
	}
	return ret;
}

void write_littleendianX( std::ofstream & stream, const int bytes, const long value)
{
	char	buf[ 4];
	
	for ( int i = 0 ; i < bytes; ++i)
	{
		buf[ i] = (char)(value >> ( i << 3));
	}
	stream.write( buf, bytes);
}

void write_littleendian( std::ofstream & stream, const int bytes, const long value)
{
	write_littleendianX( stream, bytes, (const long) value);
}

void write_littleendian( std::ofstream & stream, const int bytes, const short value)
{
	write_littleendianX( stream, bytes, (const long) value);
}

void write_littleendian( std::ofstream & stream, const int bytes, const unsigned short value)
{
	write_littleendianX( stream, bytes, (const unsigned long) value);
}

void write_littleendian( std::ofstream & stream, const int bytes, const char value)
{
	write_littleendianX( stream, bytes, (const long) value);
}

void write_littleendian( std::ofstream & stream, const int bytes, const unsigned char value)
{
	write_littleendianX( stream, bytes, (const unsigned long) value);
}

void read_littleendian( std::ifstream & stream, const int bytes, long & value)
{
	char	buf[ 4];
	stream.read( buf, bytes);
	value = 0;
	for ( int i = 0 ; i < ( bytes - 1); ++i)
	{
		value = value | ( ((long)((unsigned char)buf[i])) << (i << 3));
	}
	value = value | ( ((long)buf[(bytes-1)]) << ((bytes-1) << 3));
}

void read_littleendian( std::ifstream & stream, const int bytes, unsigned long & value)
{
	char	buf[ 4];
	stream.read( buf, bytes);
	value = 0;
	for ( int i = 0 ; i < bytes; ++i)
	{
		value = value | ( ((unsigned long)((unsigned char)buf[i])) << (i << 3));
	}
}

void read_littleendian( std::ifstream & stream, const int bytes, short & value)
{
	long	l;
	read_littleendian( stream, bytes, l);
	value = (short) l;
}

void read_littleendian( std::ifstream & stream, const int bytes, unsigned short & value)
{
	unsigned long	l;
	read_littleendian( stream, bytes, l);
	value = (unsigned short) l;
}

void read_littleendian( std::ifstream & stream, const int bytes, char & value)
{
	long	l;
	read_littleendian( stream, bytes, l);
	value = (char) l;
}

void read_littleendian( std::ifstream & stream, const int bytes, unsigned char & value)
{
	unsigned long	l;
	read_littleendian( stream, bytes, l);
	value = (unsigned char) l;
}

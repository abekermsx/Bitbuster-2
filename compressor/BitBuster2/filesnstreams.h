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

#ifndef _FILESNSTREAMS_H_
#define _FILESNSTREAMS_H_

#include <vector>

bool load_file( const char * path, std::vector<char> & data);
bool save_file( const char * path, char * data, size_t size);

#include <fstream>
void write_littleendian( std::ofstream & stream, const int bytes, const long value);
void write_littleendian( std::ofstream & stream, const int bytes, const short value);
void write_littleendian( std::ofstream & stream, const int bytes, const unsigned short value);
void write_littleendian( std::ofstream & stream, const int bytes, const char value);
void write_littleendian( std::ofstream & stream, const int bytes, const unsigned char value);
void read_littleendian( std::ifstream & stream, const int bytes, long & value);
void read_littleendian( std::ifstream & stream, const int bytes, unsigned long & value);
void read_littleendian( std::ifstream & stream, const int bytes, short & value);
void read_littleendian( std::ifstream & stream, const int bytes, unsigned short & value);
void read_littleendian( std::ifstream & stream, const int bytes, char & value);
void read_littleendian( std::ifstream & stream, const int bytes, unsigned char & value);

#endif // _FILESNSTREAMS_H_

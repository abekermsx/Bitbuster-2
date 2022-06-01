// Copyright (c) 2004 Arjan Bakker
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


// thanks to Sjoerd for some tips on the encoding of match lengths


#include "decompress.h"



ifstream infile;
ofstream outputfile;

int output_length;
unsigned char *output_data;
unsigned char *input_data;

unsigned short block_length;
int position;

int bit_counter;
int bit_data;
int byte_position;

inline int get_bit()
{
int value;

	//if 8 bit reads, time to get 8 new bits
	if ( bit_counter == 8)
	{
		//get 8 new bits
		bit_data = input_data[ byte_position++ ];

		//reset number of bits read
		bit_counter = 0;
	}

	//increase number of bits read
	bit_counter++;

	//get bit 7
	value = (bit_data & 128) >> 7;

	//shift bits
	bit_data <<= 1;

	return value;
}

unsigned short get_match_length()
{
unsigned short match_length = 1;
	
	while( get_bit() )
	{
		match_length <<= 1;
		match_length += get_bit();
		                  			
		if ( match_length == 65535 )
			return 0;
	}	
				
	match_length++;

	return match_length;
}


int decompress_block()
{
int match_offset;
unsigned short int match_length;
int match_position;

 	byte_position = 0;
 	bit_counter = 8;
 	
	while( 1 )
	{
		if ( get_bit() )
		{
			match_length = get_match_length();
			
			if ( match_length == 0 )
				return 0;
			
			match_offset = input_data[ byte_position++ ];
			
			if ( match_offset & 128 )
			{
				match_offset &= 127;
				   
				//get fourm more bits
				match_offset += get_bit() << 10;
				match_offset += get_bit() << 9;
				match_offset += get_bit() << 8;
				match_offset += get_bit() << 7;
			}	
						
			match_position = position - match_offset - 1;
			
			if ( position + match_length >= output_length )
			{
				output_length *= 2;
				
				unsigned char *tmp = new unsigned char[ output_length ];
				
				memcpy( tmp, output_data, output_length / 2 );
				
				delete [] output_data;
				
				output_data = tmp;	
			}
				
			while ( match_length-- )
				output_data[ position++ ] = output_data[ match_position++ ];
		}
		else
		{ 	
			if ( position == output_length )
			{
				output_length *= 2;
				
				unsigned char *tmp = new unsigned char[ output_length ];
				
				memcpy( tmp, output_data, output_length / 2 );
				
				delete [] output_data; 
				
				output_data = tmp;	
			}
			
			output_data[ position++ ] = input_data[ byte_position++ ];
		}	
	}
		
	return 0;
}


int decompress( string sourcename, string outputname )
{
unsigned char block_count;

	infile.open( sourcename.c_str(), ios::binary | ios::in );
	
	if ( infile.fail() )
	{
		cerr << "Failed to open " << sourcename << endl;
		return 1;
	}	
                
	// read number of file blocks
	infile.read( (char*)&block_count, 1 );
	
	if ( infile.fail() )
	{
		cerr << "Failed to read number of blocks" << endl;
		return 1;
	}	

	output_length = 32768;
	
	output_data = new unsigned char[ output_length ];
	
	position = 0;
	
	while ( block_count-- ) 
	{
		// read length of current block
		read_littleendian( infile, 2, block_length );
						
		if ( infile.fail() )
		{
			delete [] output_data;
			
			cerr << "Failed to read blocklength" << endl;
			return 1;
		}	
		
		input_data = new unsigned char[ block_length ];
	
		infile.read( (char*)input_data, block_length );
		
		decompress_block();
	
		delete [] input_data;
	}	
	
	infile.close();
	
	outputfile.open( outputname.c_str(), ios::binary | ios::out );
        
        if ( outputfile.fail() )
        {        	
        	cerr << "Failed to create " << outputname << endl;
        	return 1;
        }
	
	outputfile.write( (char*)output_data, position );
	
	outputfile.close();
	
	delete [] output_data;

	return 0;
}


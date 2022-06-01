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


#include "encode.h"


ofstream *outfile;                       	// stream where compressed data will be written to
unsigned char bitdata;                  	// current bit status of bitstream
int bitcount;                           	// number of bits written to current byte
const int buffer_length = 64;           	// size of buffer for compressed data
unsigned char outbuffer[ buffer_length ];	// buffer with compressed data
int buffer_position;                    	// current position to write new bytes to
int bit_position;                       	// current position to write bit data to


// flush data up to the current bit position to outfile
void flush_buffer()
{
int position = 0;

	// write all data before bit position
	outfile->write( (char*)outbuffer, bit_position );

	// set new position to write bytes to
	buffer_position = buffer_position - bit_position;

	// move all data from bit position till end to the beginnen of the buffer
	while ( bit_position < buffer_length )
		outbuffer[ position++ ] = outbuffer[ bit_position++ ];

	// reset bit position
	bit_position = 0;
}


// flush all data from the buffer
void flush_rest()
{
 	// write data, if any
	if ( buffer_position )
		outfile->write( (char*)outbuffer, buffer_position );

	// reset buffer / bit positions
	buffer_position = bit_position = 0;
}


// write byte to buffer
void write_byte( unsigned char value )
{
 	// put byte in stream
	outbuffer[ buffer_position++ ] = value;

	// flush buffer, if full
	if ( buffer_position == buffer_length )
		flush_buffer();
}


// write a bit to the buffer
void write_bit( int value )
{
	//if 8 bits has been added
	if ( bitcount == 8 )
	{	
		// set new but position
		bit_position = buffer_position++;

		// if buffer full, flush data
		if ( buffer_position == buffer_length )
			flush_buffer();

		// reset bit count
		bitcount = 0;
	}

	//shift value
	bitdata <<= 1;		

	//set bit if value is non-zero
	if ( value )
		bitdata++;		

	// write the bitdata to the buffer
	outbuffer[ bit_position ] = bitdata;
		
	//increase number of bits added to value
	bitcount++;			
}


// write value times bits to buffer
void write_bits( int value, int bits )
{
	// write bits to buffer
	for ( int c = 0; c < bits; c++ )
		write_bit( value );
}




// write encoded value to stream
void write_length( int value )
{
unsigned int mask = 32768;

	++value;
	
	// find highest bit set
	while ( !( value & mask ) )
		mask >>= 1;
		
	while( true )
	{
		// if mask equals 1, last bit has been written
		if ( mask == 1 )
		{
			write_bit( 0 );
			return;
		}
		
     		mask >>= 1;

     		// more data
     		write_bit(1);
		    
		// write data bit
		write_bit( value & mask );
	}
}

// output compressed data to file name
void write_file( ofstream *p_outfile, unsigned char *data, int length, match_result *match_results  )
{
int position = 0;
int offset;

	bitcount = bitdata = 0;

	outfile = p_outfile;

	// set bit / buffer position
	buffer_position = 1;
	bit_position = 0;

	// loop while there's more compressed data to process
	while ( position < length )
	{
		// if it's a match
		if ( match_results[ position ].first > 1 )
		{        
			// write match marker
			write_bit( 1 );
			
			// write length of offset
			write_length( match_results[ position ].first - 2 );
			
			// calculate offset
			offset = position - match_results[ position ].second - 1;
                        
			// if long offset
			if ( offset > 127 )
			{
				// write lowest 7 bits of offset, plus long offset marker
				write_byte( offset | 128 ); 
				
				// write remaing offset bits
				write_bit( offset & 1024 );
				write_bit( offset & 512 );
				write_bit( offset & 256 );
				write_bit( offset & 128 );
			}
			else
			{
				// write short offset
				write_byte( offset );

			}

			// updated current position in compressed data
			position += match_results[ position ].first;
		}
		else
		{
			// write literal byte marker
			write_bit( 0 );
			
			// write literal byte
			write_byte( data[ position++ ] );
		}
	}

	
	// write RLE marker
	write_bit( 1 );		
	
	// write eof marker
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );   
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );  
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );   
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );  
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );   
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );  
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );   
	write_bit( 1 ); write_bit( 1 ); write_bit( 1 ); write_bit( 1 );    
			
	// write all remaining data
	flush_rest();
}



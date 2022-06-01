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



#include "compress.h"
#include "encode.h"


unsigned char *data;		// data to crunch
int length;			// length of data to crunch

// matches for all positions in the data
match_result *match_results;

// struct to store the location of a 2 byte combo 
typedef struct occur
{
	int position;		// location of 2 byte combo
	occur *previous;        // link to previous combo
} occur;


occur *occur_ptr[ 65536 ];      // there are 2^16 possible combo's
occur *occur_pool;             	// array with free occur structs
int occur_index = 0;		// position of next free occur struct

// return the next occur struct (doing so is faster than using new/delete all the time
occur *get_occur()
{
	return &occur_pool[ occur_index++ ];
}


// return length of opened file
int get_file_length( ifstream & file )
{
int current_position;
int length;

	// not opened file?
	if ( file.fail() )
		return -1;

	// get current file position
	current_position = file.tellg();

	// move to end of file
	file.seekg( 0, ios::end );

	// get current file position (=length of file)
	length = file.tellg();

	// set file position back to original position
	file.seekg( current_position, ios::beg );

	// return found filelength
	return length;
}


// get length of match from current position
int get_match_length( int new_data, int previous_data )
{
int match_length = 2;

	// increase match length while data matches
	while ( ( new_data < length ) && ( data[ new_data++ ] == data[ previous_data++ ] ) )
		match_length++;

	return match_length;
}


// get position of a 2-byte pair
int get_pair( int position )
{
	return data[ position ] + ( data[ position + 1 ] << 8 );
}


// return longest match
match_result get_best_match( int position )
{
match_result best_result;
int temp_match_length;
int pair;
occur *cur_occur;

	best_result.first = 0;		// match length
	best_result.second = -1;	// position

	// get 2 byte pair value
	pair = get_pair( position );

	// get closest match
	cur_occur = occur_ptr[ pair ];

	// keep searching if more matches found
	while ( cur_occur != NULL )
	{
		// if match is still in range of maximum offset
		if ( cur_occur->position > ( position - 0x07ff ) ) 
		{
			// get match length
			temp_match_length = get_match_length( position + 2, cur_occur->position + 2 );

			// if better than best result
			if ( temp_match_length > best_result.first )
			{
				// copy new match result
				best_result.first = temp_match_length;
				best_result.second = cur_occur->position;
			}

			// move to previous match
			cur_occur = cur_occur->previous;
		}
		else
			break;	// match out of range
	}

	return best_result;
}


// find matches for all data
void find_matches()
{
int pair;
occur *cur_occur;
match_result result;
int position = 0;
    
	occur_index = 0;	// reset position of first free occur struct
	
	//loop through all data, except last element
	//since it can't be the start of a value pair
	while ( position < length - 1 )
	{
		// get match data for current position
		match_result best_result = get_best_match( position );

		// get 2 byte pair value
		pair = get_pair( position );

		// get a new occurance
		cur_occur = get_occur();
		
		// keep a link to previous match
		cur_occur->previous = occur_ptr[ pair ];	
		
		cur_occur->position = position;
                
                // store closest occurance
		occur_ptr[ pair ] = cur_occur;
		
		// if match found
		if ( best_result.first > 1 )
		{
			result = best_result;

			// write match length / position in match area
			for ( int i = position; i < position + best_result.first; i++)
			{			
				match_results[ i ].first = result.first--;	// match length
				match_results[ i ].second = result.second++;	// match position
			}
			   
			// RLE match found?
			if ( best_result.second == ( position - 1 ) )
			{
				// only skip data if a long match found
				if ( best_result.first > 16 )
					position += best_result.first - 16;
			}   
		}
		
		// continue with next data
		position++;
	}
}


// struct for storing cost / position at current node in search tree
typedef struct match_link
{
	int cost;		// cost in bits to get at this position
	int position;           // current position
	match_link *parent;  	// pointr to parent node
	match_link *literal;    // pointer to next literal data node
	match_link *match;      // pointer to next match data node
} match_link;


int depth;		// current search depth
match_link *best;       // pointer to end of best path


// a pool for getting match_link structs, faster than using new / delete 
match_link match_link_pool[ 65536 ];
int match_link_index;



// get encoding size of value
int get_gamma_size( int value )
{
int gamma_size = 1;

	// increase size if there's still bits left after shifting one bit out
	while ( value )
	{
		value--;

		gamma_size += 2;	// each time 2 extra bits are needed
 
		value >>= 1;
	}

	// return calculated gamma size
	return gamma_size;
}


int *cost;


void slash_matches()
{
int position = length - 1;

	while ( position > 0 )
	{
	
		if ( match_results[ position ].first > 1 )
		{
			cost[ position ] = cost[ position + match_results[ position ].first ] +
				( ( position - match_results[ position ].second ) > 128 ? 12 : 8 ) + 
					get_gamma_size(	match_results[ position ].first - 2 ) + 1;
					
			if ( cost[ position + 1 ] + 9 <= cost[ position ] )
			{
				cost[ position ] = cost[ position + 1 ] + 9;
				
				match_results[ position ].first = 0;
			}
		}
		else
		{	
			cost[ position ] = cost[ position + 1 ] + 9;		
		}	   
			
		position--;
	}	

}

// compress a file
void compress( const string & file, const string & output_file, unsigned short block_length  )
{
int compressed_length = 0;
int file_length;
ofstream outfile;
ifstream infile;
unsigned char block_count;
unsigned int position;
int remaining_length;
	 	
	// open file that has to be compressed
	infile.open( file.c_str() , ios::binary | ios::in );
	
	// display error message if opening failed
	if ( infile.fail() )
	{
		cerr << "Failed to read " << file << endl;	
		return ;
	}	

	// get fil length
	remaining_length = file_length = get_file_length( infile );
         		 
	// empty files won't be compressed
	if ( file_length == 0 )
	{
		cerr << "Nothing to compress!" << endl;
		return;
	}	

	if ( ( file_length - 1 ) / block_length + 1 > 256 )
	{
		cerr << output_file << " is too big to compress with this block length." << endl;
		return;
	}

	// create output file
    outfile.open( output_file.c_str(), ios::binary | ios::out );
	       
	// displaty error message if creation failed
	if ( outfile.fail() )
	{
		cerr << "Failed to open " << output_file << " for output" << endl;
		return;
	}

	// calculate number of blocks to process
	block_count = ( file_length - 1 ) / block_length + 1;

	// write number of blocks to file
	outfile.write( (char*)&block_count, 1 );
						             	   
 	cout << "Compressing: " << file << "... ";
 	cout.flush();
 	
 	// process all blocks
 	while ( block_count-- )
 	{ 		 
 		// get current position in file
	 	position = outfile.tellp();
		
		// write dummy length value
		outfile.write( (char*)&position, 2 );
				 	
		// set length of current block	
 		if ( remaining_length < block_length )
 			length = remaining_length;
 		else
 			length = block_length;
 			
		// mark each value pair as non-occurring yet
		for ( int i = 0; i < 256 * 256; i++)
			occur_ptr[ i ] = NULL;
	
		// new space for data
		data = new unsigned char[ length ];
		
		// read block
		infile.read( (char*)data, length );
	
		// create new array for storing match results
		match_results = new match_result[ length ];
	
		// create new pool for occurances
		occur_pool = new occur[ length ];
		
		cost = new int[ length + 1];
		
		// reset match results
		for ( int j = 0; j < length; j++)
		{
			match_results[ j ].first = -1;
			match_results[ j ].second = -1;
			
			cost[j] = 0;
		}
	
		cost[ length ] = 0;
	
		// find matches for the whole file
		find_matches();
	                      	        
		// remove all bad matches
		slash_matches();
	        
		// write compressed data
		write_file( &outfile, data, length, match_results );
	          		                            		           
	    // remove data
	    delete cost;
		delete occur_pool; 	
		delete match_results;
		delete data;
		
		// get current position in file
		compressed_length = outfile.tellp();
		
		// move back
		outfile.seekp( position, ios::beg );
		
		// calculate length of decompressed data
		position = compressed_length - position - 2;
		
		// write length to file
		write_littleendian( outfile, 2, (long)position );
				
		// back to end of file
		outfile.seekp( 0, ios::end );
		
		// decrease number of bytes to go
		remaining_length -= block_length;
	}
	
	// close files
	infile.close(); 
	outfile.close();
	
	// output statistics if writing file succeeded
	if ( compressed_length > 0 )
		cout << file_length << " -> " << compressed_length << endl;	 
}



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

#include <string.h>
#include "OptionParser.h"

OptionParser::OptionParser( int argc, char ** argv)
{
	m_argc = argc;
	m_argv = argv;
	m_argi = 1;
	m_chari = 0;
	m_optarg = NULL;
	m_nonoption_begin = 0;
}

int OptionParser::GetOption( const char * options)
{	
	int	option = -1;
	if ( m_argi < m_argc)
	{
		if ( m_chari == 0)
		{
			if ( m_argv[ m_argi][0] == '-')
			{
				m_chari = 1;
			}
			else
			{
				// A non-option found
				m_nonoption_begin = m_nonoption_begin ? m_nonoption_begin : m_argi;
				for ( ; m_argi < m_argc && m_argv[ m_argi][0] != '-'; ++m_argi);
				m_nonoption_end = m_argi;
				if ( m_argi != m_argc)
				{
					m_chari = 1;
				}
			}			
		}
		if ( m_chari != 0)
		{
			// Process next option
			m_option = m_argv[ m_argi][ m_chari];
			char * option_syntax = strchr( (char*)options, m_option);
			if ( option_syntax)
			{			
				option = m_option;
				if ( option_syntax[ 1] == ':')
				{
					m_optarg = NULL;
					if ( m_argv[ m_argi][ m_chari + 1] != '\0')
					{
						m_optarg = &m_argv[ m_argi][ m_chari+1];
					}
					else
					{
						if ( option_syntax[ 2] != ':')
						{
							// NOT an optional argument
							if ( (m_argi + 1) < m_argc)
							{
								m_optarg = m_argv[ ++m_argi];
							}
							else
							{
								// Missing argument
								option = '!';
							}
						}
					}					
					m_chari = strlen( m_argv[ m_argi]) - 1;
				}
			}
			else
			{
				// Unrecognized option
				option = '?';
			}
			m_chari++;
			if ( m_argv[ m_argi][ m_chari] == '\0')
			{
				m_argi++;
				m_chari = 0;
			}
			if ( m_chari == 0 && m_nonoption_begin != 0)
			{
				// Move option argvs from behind to in front of non-option argvs
				int	bottom = m_nonoption_begin;
				int middle = m_nonoption_end;					
				int top = m_argi;

				while ( top > middle && middle > bottom)
				{
					if ( top-middle > middle-bottom)
					{
						// Move smaller bottom part to last part of top
						const int len = middle-bottom;
						for ( int i = 0 ; i < len; ++i)
						{
							char * argv = m_argv[ bottom + i];
							m_argv[ bottom + i] = m_argv[ top - len + i];
							m_argv[ top - len + i] = argv;
						}
						top -= len;
					}
					else
					{
						// Move smaller top part to begin of bottom
						const int len = top-middle;
						for ( int i = 0 ; i < len; ++i)
						{
							char * argv = m_argv[ bottom + i];
							m_argv[ bottom + i] = m_argv[ middle + i];
							m_argv[ middle + i] = argv;
						}
						bottom += len;
					}
				}										
				m_nonoption_begin = m_argi - ( m_nonoption_end - m_nonoption_begin);
				m_nonoption_end = m_argi;
			}
		}
	}

	return option;
}

int OptionParser::Option( void) const
{
	return m_option;
}

int OptionParser::Index( void) const
{
	return m_nonoption_begin ? m_nonoption_begin : m_argc;
}

char * OptionParser::Argument( void) const
{
	return m_optarg;
}

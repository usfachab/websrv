#include "./include/include.hpp"

size_t eofChunk( size_t startPos, std::string& chunk )
{
	bool cr = false;
	bool fl = false;

	size_t crPos = 0;
	size_t flPos = 0;

	for ( int i = startPos; i < chunk.length(); i++ )
	{
		if ( chunk.at( i ) == '\r' )
		{
			cr = !cr;
			crPos = i;
		}
		else if ( chunk.at( i ) == '\n' )
		{
			fl = !fl;
			flPos = i;
		}
		if ( cr == true && fl == true && flPos == crPos + 1 )
			return ( crPos );
	}
		return ( std::string::npos );
}

void	handleChunks( std::string buffer )
{
	long		chunkSize;
	size_t		crflPos ( 0 );
	std::string	line;

	for ( int i = 0; i < buffer.length(); i++ )
	{
		crflPos = eofChunk( crflPos, buffer );
		if ( crflPos != std::string::npos )
		{
			line = buffer.substr( i, crflPos);
			i = crflPos;
			crflPos += 2;
			COUT( line );
		}
		i++;
	}
}

int main()
{
	std::string request( "B\r\nHello world\r\nE\r\nhow are my man\r\n0\r\n\r\n" );
	handleChunks( request );

	return 0;
}
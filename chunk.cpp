#include "./include/include.hpp"

void	handleChunks( int fd, std::string buffer, bool& Continue, long& chunkSize )
{
	static int	count = 0;
	std::string	hex;

	for ( int i = 0; i < buffer.length(); ++i )
	{
		if ( Continue == false )
		{
			count = 0;
			while ( i < buffer.length() && buffer[ i ] != '\r' )
			{
				hex.push_back( buffer[ i ] );
				i++;
			}
			if ( !hex.empty() )
			{
				chunkSize = std::strtol( hex.c_str(), NULL, 16 );
				hex.clear();
				i += 2;
				if ( chunkSize == 0 )
					exit( 0 ) ;
			}
		}
		while ( i < buffer.length() )
		{
			if ( buffer[ i ] == '\r' )
			{
				Continue = false;
				i += 1;
				chunkSize = 0;
				count = 0;
				break ;
			}
			write( 1, &buffer[ i ], 1 );
			count++;
			i++;
		}
		if ( count < chunkSize )
			Continue = true;
	}
}

int main()
{
	int fd = open( "./infile", O_RDONLY );
	int output = open( "./output", O_CREAT | O_RDWR, 0666 );
	long chunkSize = 0;
	char buffer[ 256 ];
	bool Continue = false;

	while ( true )
	{
		int rd = read( fd, buffer, 255 );
		if ( rd > 0 )
		{
			buffer[ rd ] = 0;
			handleChunks( output,  buffer, Continue, chunkSize);
		}
		if ( rd <= 0 )
			break;
	}
	return 0;
}
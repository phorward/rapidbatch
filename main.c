
/*
 * Includes
 */
#include "rb_global.h"

/*
 * Global variables
 */
BOOLEAN REGEX_ENABLED = TRUE;
BOOLEAN CASE_INSENSITIVITY = TRUE;

/*
 * Functions
 */

int main( int argc, char** argv )
{
	char*		filecont		= (char*)NULL;

	rb_register_std();
	rb_add_native_function( "print", print, FALSE, "v+", RB_FUNC_AT_RUNTIME );

	if( argc <= 1 )
	{
		fprintf( stderr, "%s: Missing filename!\n", *argv );
		return EXIT_FAILURE;
	}

	switch( rb_read_file( &filecont, argv[1] ) )
	{
		case 1:
			fprintf( stderr, "%s: File not found: %s\n", *argv, argv[1] );
			return EXIT_FAILURE;

		case 0:
			break;

		default:
			return EXIT_FAILURE;
			break;
	}

	if( !rb_comp_compile( argv[1], filecont ) )
	{
		fprintf( stderr, "--- Running program ---\n" );
		rb_run( );
	}

	return EXIT_SUCCESS;
}


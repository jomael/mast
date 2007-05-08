/*
 *  MAST: Multicast Audio Streaming Toolkit
 *
 *  By Nicholas J. Humfrey <njh@ecs.soton.ac.uk>
 *
 *  Copyright (C) 2003-2006 University of Southampton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 */

#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "config.h"
#include "mast.h"





char* mast_gethostname()
{
	char hostname[ HOST_NAME_MAX ];
	char domainname[ DOMAIN_NAME_MAX ];
	struct hostent	*hp;
	
	if (gethostname( hostname, HOST_NAME_MAX ) < 0) {
		// Failed
		return NULL;
	}
	
	// If it contains a dot, then assume it is a FQDN
    if (strchr(hostname, '.') != NULL)
		return strdup( hostname );

    // Try resolving the hostname into a FQDN
    if ( (hp = gethostbyname( hostname )) != NULL ) {
    	if (strchr(hp->h_name, '.') != NULL)
    		return strdup( hp->h_name );
    }

	// Try appending our domain name
	if ( getdomainname( domainname, DOMAIN_NAME_MAX) == 0
	     && strlen( domainname ) )
	{
		int fqdn_len = strlen( hostname ) + strlen( domainname ) + 2;
		char *fqdn = malloc( fqdn_len );
		snprintf( fqdn, fqdn_len, "%s.%s", hostname, domainname );
		return fqdn;
	}


	// What else can we try?
	return NULL;
}



// Handle an error and store the error message
void mast_message_handler( int level, const char* file, int line, char *fmt, ... )
{
	va_list args;
	FILE *fd = stderr;
	
	// If debugging is enabled then display the filename and line number
#ifdef DEBUGGING
	fprintf( fd, "[%s:%d] ", file, line );
#endif
	
	// Display the error message
	switch( level ) {
		case MSG_LEVEL_DEBUG:	fprintf( fd, "Debug: " ); break;
		case MSG_LEVEL_INFO:	fprintf( fd, "Info: " ); break;
		case MSG_LEVEL_WARNING:	fprintf( fd, "Warning: " ); break;
		case MSG_LEVEL_ERROR:	fprintf( fd, "Error: " ); break;
		case MSG_LEVEL_FATAL:	fprintf( fd, "Fatal Error: " ); break;
	}
	
	// Display the message
	va_start( args, fmt );
	vfprintf( fd, fmt, args );
	fprintf( fd, ".\n" );
	va_end( args );
	
	// If it is an error, then stop
	if (level==MSG_LEVEL_FATAL) {
		exit(-1);
	}
}


// Parse a string into a numeric payload type
int mast_parse_payloadtype( char* ptstr )
{

	// If it is an integer, just return it
	return 11;

	// Error
	return -1;
}








static int mast_running = TRUE;

static void mast_termination_handler(int signum)
{
	mast_running = FALSE;
	switch(signum) {
		case SIGTERM:	fprintf(stderr, "Got termination signal.\n"); break;
		case SIGINT:	fprintf(stderr, "Got interupt signal.\n"); break;
	}
}

int mast_still_running()
{
	return mast_running;
}

void mast_setup_signals()
{
	// Setup special handling of signals
	signal(SIGTERM, mast_termination_handler);
	signal(SIGINT, mast_termination_handler);
	//signal(SIGHUP, mast_termination_handler);

	// Set this 
	mast_running = TRUE;

}


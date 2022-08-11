/*-------------------------------------------------------------------------*
 *---									---*
 *---		wordClientServer.h					---*
 *---									---*
 *---	    This file declares C functions and constants common to both	---*
 *---	wordClient.c and wordServer.c.					---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

//---		Header file inclusion					---//

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>	// For unlink()
#include	<sys/types.h>	// For waitpid(), opendir()
#include	<sys/wait.h>	// For waitpid()
#include	<dirent.h>	// For opendir(), readdir(), closedir()
#include	<sys/socket.h>	// For socket()
#include	<netinet/in.h>	// For sockaddr_in and htons()
#include	<netdb.h>	// For getaddrinfo()
#include	<errno.h>	// For errno var
#include	<sys/stat.h>	// For open(), read(),write(), stat()
#include	<fcntl.h>	// and close()


//---		Definition of constants:				---//

#define		BUFFER_LEN		512

#define		DIR_CMD_CHAR		'l'

#define		WRITE_CMD_CHAR		'w'

#define		SPELL_CMD_CHAR		's'

#define		DELETE_CMD_CHAR		'd'

#define		QUIT_CMD_CHAR		'q'

const int	MIN_FILE_NUM		= 0;

const int	MAX_FILE_NUM		= 63;

#define		SPELLER_PROGNAME	"/usr/bin/aspell"

#define		SPELLER_PROG_OPTION	"list"

#define		ENDING_TEXT		"xxxyyyzzz"

#define		FILENAME_EXTENSION	".txt"

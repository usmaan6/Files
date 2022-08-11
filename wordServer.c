
//	Compile with:
//	$ gcc wordServer.c -o wordServer -lpthread

//---		Header file inclusion					---//

#include	"wordClientServer.h"
#include	<pthread.h>	// For pthread_create()


//---		Definition of constants:				---//

#define		STD_OKAY_MSG		"Okay"

#define		STD_ERROR_MSG		"Error doing operation"

#define		STD_BYE_MSG		"Good bye!"

#define		THIS_PROGRAM_NAME	"wordServer"

#define		OUTPUT_FILENAME		"out.txt"

#define		ERROR_FILENAME		"err.txt"

const int	ERROR_FD		= -1;


//---		Definition of functions:				---//

void handleClient (void* vPtr)
{
  int* numArra = (int*)vPtr;
  int clientFd = numArra[0];
  int threadCount = numArra[1];

  free(numArra);

  //  II.B.  Read command:
  char	buffer[BUFFER_LEN];
  char	command;
  int	fileNum;
  char	text[BUFFER_LEN];
  int 	shouldContinue	= 1;

  while  (shouldContinue)
  {
    text[0]	= '\0';

    read(clientFd,buffer,BUFFER_LEN);
    printf("Thread %d received: %s\n",fileNum,buffer);
    sscanf(buffer,"%c %d \"%[^\"]\"",&command,&fileNum,text);

    // YOUR CODE HERE

    if (command == DIR_CMD_CHAR)
      {
	DIR* dirPtr = opendir(".");
	struct dirent* entryPtr;

	while ( (entryPtr = readdir(dirPtr)) != NULL)
	  {
	    strcat(buffer, entryPtr->d_name);
	    strcat(buffer, "\n");
	  }

	write(clientFd,buffer,strlen(buffer));
	closedir(dirPtr);
      } else if (command == WRITE_CMD_CHAR)
      {
	char filename[BUFFER_LEN];
	snprintf(filename,BUFFER_LEN,"%d.txt",fileNum);
	//	printf("writeCmd, before sizing text: text = %s \n", &text);
	//        int sourceFd = open(buffer,O_RDONLY);
	int destFd = open(filename,O_WRONLY|O_CREAT|O_TRUNC, 0660);
	int numBytes;
	size_t textLen = strlen(text);
	//	printf("clientFd = %d, fileNum = %d, text = %s, textLen = %d \n",clientFd,fileNum,text, textLen);

	      	if (destFd > 0)
	  	  {
	    	    write(destFd,text,strlen(text));
		    close(destFd);
		    
		  } else {
		  write(clientFd,STD_ERROR_MSG,sizeof(STD_ERROR_MSG) - 1);
		}
	   write(clientFd,STD_OKAY_MSG,sizeof(STD_OKAY_MSG) - 1);
      } else if (command == DELETE_CMD_CHAR)
      {
	char filename[BUFFER_LEN];
	snprintf(filename,BUFFER_LEN,"%d.txt",fileNum);

	if (unlink(filename) > 0) {
	  write(clientFd,STD_ERROR_MSG,sizeof(STD_ERROR_MSG) - 1);
	} else {
	  write(clientFd,STD_OKAY_MSG,sizeof(STD_OKAY_MSG) - 1);
	}


      } else if (command == QUIT_CMD_CHAR)
      {
	shouldContinue = 0;
	write(clientFd,STD_BYE_MSG,sizeof(STD_BYE_MSG) - 1);

      } else if (command == SPELL_CMD_CHAR)
      {
	char filename[BUFFER_LEN];
	snprintf(filename,BUFFER_LEN,"%d.txt",fileNum);

      
    
	int destFd = open(filename,O_RDWR);
	if(destFd < 0) {
	  write(clientFd,STD_ERROR_MSG,sizeof(STD_ERROR_MSG));
	} 
	else {
	  long length	= lseek(destFd,0,SEEK_END);
	  int  status	= write(destFd,ENDING_TEXT,sizeof(ENDING_TEXT)-1);
	
	  int fromSpeller[2];
	  if (pipe(fromSpeller) < 0) {
            write(clientFd,STD_ERROR_MSG,sizeof(STD_ERROR_MSG));
	  }
	  else{
            int pid = fork();
            if(pid == 0) {
                lseek(destFd,0,SEEK_SET);
                dup2(destFd,STDIN_FILENO);
                dup2(fromSpeller[1],STDOUT_FILENO);
                execl(SPELLER_PROGNAME,SPELLER_PROGNAME, SPELLER_PROG_OPTION,NULL);
                write(fromSpeller[1],STD_ERROR_MSG,sizeof(STD_ERROR_MSG));
                write(fromSpeller[1],"\n",1);
                write(fromSpeller[1],ENDING_TEXT,BUFFER_LEN);
                exit(EXIT_FAILURE);
      	    }
            char text[BUFFER_LEN];
            memset(buffer,'\0',BUFFER_LEN);
	    memset(text,'\0',BUFFER_LEN);
	  
	    int numBytes;

            while((numBytes =  read(fromSpeller[0],buffer,BUFFER_LEN)) > 0) {
	      strncat(text,buffer,numBytes);
	      if(strstr(text,ENDING_TEXT) != NULL)
		break;
	    }
        
            while  ((pid = waitpid(-1,NULL,WNOHANG)) > 0)
	      {}
        
            write(clientFd,text,BUFFER_LEN);
            ftruncate(destFd,length);
            close(destFd);
            close(fromSpeller[0]);
            close(fromSpeller[1]);
        }
    }









      }
       
	
  }  
}  

//  PURPOSE:  To run the server by 'accept()'-ing client requests from
//	'listenFd' and doing them.
void		doServer	(int		listenFd
				)
{
  //  I.  Application validity check:

  //  II.  Server clients:
  pthread_t		threadId;
  pthread_attr_t	threadAttr;
  int			threadCount	= 0;


  pthread_attr_init(&threadAttr);
  pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED);

  while (1) {

    int*  numArray = (int*)calloc(2,sizeof(int));
    
    
    numArray[0] = accept(listenFd,NULL,NULL);
    numArray[1] = threadCount++;
    pthread_create(&threadId,&threadAttr,handleClient,(void*)numArray);

  }

  pthread_attr_destroy(&threadAttr);
      
  
  

  
}


//  PURPOSE:  To decide a port number, either from the command line arguments
//	'argc' and 'argv[]', or by asking the user.  Returns port number.
int		getPortNum	(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Get listening socket:
  int	portNum;

  if  (argc >= 2)
    portNum	= strtol(argv[1],NULL,0);
  else
  {
    char	buffer[BUFFER_LEN];

    printf("Port number to monopolize? ");
    fgets(buffer,BUFFER_LEN,stdin);
    portNum	= strtol(buffer,NULL,0);
  }

  //  III.  Finished:  
  return(portNum);
}


//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//	to the OS telling this server when a client process has connect()-ed
//	to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int		getServerFileDescriptor
				(int		port
				)
{
  //  I.  Application validity check:

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET, // AF_INET domain
			        SOCK_STREAM, // Reliable TCP
			        0);

  if  (socketDescriptor < 0)
  {
    perror(THIS_PROGRAM_NAME);
    return(ERROR_FD);
  }

  //  II.B.  Attempt to bind 'socketDescriptor' to 'port':
  //  II.B.1.  We'll fill in this datastruct
  struct sockaddr_in socketInfo;

  //  II.B.2.  Fill socketInfo with 0's
  memset(&socketInfo,'\0',sizeof(socketInfo));

  //  II.B.3.  Use TCP/IP:
  socketInfo.sin_family = AF_INET;

  //  II.B.4.  Tell port in network endian with htons()
  socketInfo.sin_port = htons(port);

  //  II.B.5.  Allow machine to connect to this service
  socketInfo.sin_addr.s_addr = INADDR_ANY;

  //  II.B.6.  Try to bind socket with port and other specifications
  int status = bind(socketDescriptor, // from socket()
		    (struct sockaddr*)&socketInfo,
		    sizeof(socketInfo)
		   );

  if  (status < 0)
  {
    perror(THIS_PROGRAM_NAME);
    return(ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor,5);

  //  III.  Finished:
  return(socketDescriptor);
}


int		main		(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Do server:
  int 	      port	= getPortNum(argc,argv);
  int	      listenFd	= getServerFileDescriptor(port);
  int	      status	= EXIT_FAILURE;

  if  (listenFd >= 0)
  {
    doServer(listenFd);
    close(listenFd);
    status	= EXIT_SUCCESS;
  }

  //  III.  Finished:
  return(status);
}

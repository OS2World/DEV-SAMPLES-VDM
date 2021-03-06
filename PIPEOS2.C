/**********************************************************/
/**********************************************************/
/***                                                    ***/
/***  Program name: PIPEOS2.C                           ***/
/***                                                    ***/
/***  Created     : 16th May 1990                       ***/
/***                                                    ***/
/***  Revised     : 26th February 1992                  ***/
/***                                                    ***/
/***  Author      : Tim Sennitt, Dorle Hecker           ***/
/***                                                    ***/
/***  Purpose     : To demonstrate the use of an OS/2   ***/
/***                created named pipe connecting to    ***/
/***                many DOS sessions                   ***/
/***                                                    ***/
/***  Compile     : icc /O+ pipeos2.c                   ***/
/***  or          : cl386 pipeos2.c                     ***/
/***                                                    ***/
/***  Input param : A number between 1 and 255          ***/
/***                (number of pipe instances)          ***/
/***                                                    ***/
/**********************************************************/

/**********************************************************/
/***  DEFINES                                           ***/
/**********************************************************/
#define INCL_DOS
#define INCL_DOSNMPIPES
/**********************************************************/
/***  INCLUDES and VARIABLES                            ***/
/**********************************************************/
#include <os2.h>
#include <stdlib.h>
#include <string.h>

#ifdef __IBMC__
   void     _System NewThread(ULONG threadArg);
#else
   void     NewThread(ULONG threadArg);
#endif
   TID      threadID[512];
   HPIPE    piphand[255];
   ULONG    threadArg, threadFlags, stack_size;
   ULONG    outbuffer, inbuffer, timeout, BytesWrit;
   USHORT   rc, loopsize, i;
   char     prep_string[11];
/**********************************************************/
/***  MAIN PROGRAM                                      ***/
/**********************************************************/
main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{
   BOOL fEnd_Correct=FALSE;
   threadFlags = 0;          /* start thread immediatly    */
   stack_size  = 1024;       /* give stack size in bytes   */
   threadArg   = 1;

   if ( argc != 2 || (loopsize = atoi(argv[1])) == 0 )
     { printf("You have not specified the correct bacon size !\n");
       printf("The syntax is PIPEOS2 NNNN (where NNNN is a number between 1 and 255)\n");
       exit(0);
      } /*end-if*/
   for (i = 1; i < loopsize+1; i++)
     {
       rc = DosCreateThread(&threadID[i], NewThread, i,
                            threadFlags, stack_size);
       if (rc != 0)
         { printf("DosCreateThread error = %d\n",rc);
          exit (1);
         } /*end-if*/
       printf("Pipe-Thread number %d created\n",i);
       printf("Please start the DOS client\n");
     } /*end-for*/

   printf("Now lets send some data to it......\n");

   /****************************************************************/
   /* At this point we will loop getting keyboard input            */
   /* and writing this to our named pipe (until we enter null)     */
   /****************************************************************/
   printf("ENTER\n [B]lue, [C]yan, [G]reen, [P]urple, \
[R]ed, [W]hite, [Y]ellow or [Q]uit\n");
   gets(prep_string);
   while (prep_string[0] != 0)
     {
      if (prep_string[0] == 'q' || prep_string[0] == 'Q')
        { for (i = 1; i < loopsize+1; i++)
             { rc = DosWrite(piphand[i],
                             (PVOID)prep_string,
                             strlen(prep_string),
                             &BytesWrit);
               if (rc !=0) printf("Return code from DosWrite         = %d\n",rc);
             } /* end-for */
          prep_string[0]=0; fEnd_Correct=TRUE;
        }
      else
        { for (i = 1; i < loopsize+1; i++)
             {
               rc = DosWrite(piphand[i],
                             (PVOID)prep_string,
                             strlen(prep_string),
                             &BytesWrit);
               if (rc !=0) printf("Return code from DosWrite         = %d\n",rc);
              } /* end-for */
          printf("ENTER\n [B]lue, [C]yan, [G]reen, [P]urple, \
[R]ed, [W]hite, [Y]ellow or [Q]uit\n");
          gets(prep_string);

        } /* endif */
     } /* endwhile */

   if (!fEnd_Correct)
     { prep_string[0]='q';
       rc = DosWrite(piphand[i],
                     (PVOID)prep_string,
                     strlen(prep_string),
                     &BytesWrit);
       if (rc !=0) printf("Return code from DosWrite         = %d\n",rc);
     }
 exit(0);
}

/****************************************************************/
/* This is our thread process which creates N named pipes then  */
/* waits for the DOS sessions to connect to them.               */
/****************************************************************/
void NewThread(ULONG threadArg)
{

outbuffer = 25;
inbuffer  = 25;
timeout   = 200;

  rc = DosCreateNPipe("\\PIPE\\TIMSP\0",      /* create pipe    */
                      &piphand[threadArg],
                      0x4081,
                      0x0008,
                      outbuffer,
                      inbuffer,
                      timeout);
  if (rc != 0)
    { DosBeep(300,200); DosBeep(100,200);
      exit(1);
    }
  DosBeep(300,200); DosBeep(600,200);

/****************************************************************/
/* now we wait for our DOS session to connect to us             */
/****************************************************************/

  rc = DosConnectNPipe(piphand[threadArg]);
  if (rc != 0)
    { DosBeep(100,200);
      exit(1);
    }
  DosBeep(600,200);
  printf("DOS Session number %d connected\n",threadArg);
}


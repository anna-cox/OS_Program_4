// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

#define CLOSED 0                //file is CLOSED

#define OPEN 1                  //file is open

void info(FILE *fp);



int main()
{
  //keeps track if the file is open or closed
  int filestat = CLOSED;
  FILE* fp;


  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {

    // Print out the msh prompt
    printf ("msh> ");


    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ )
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );
    }

    //start here

    //The open command, takes the file name as the parameter, if file is already open
    //or file not found prints an error and gets ready for another command
    if(strcmp(token[0],"open")==0)
    {
        //file already open
        if(filestat == OPEN)
            printf("Error: File system image already open.\n");
        else
        {

            fp = fopen(token[1], "r");

            //file not found
            if(fp == NULL)
                printf("Error: File system image not found.\n");
            else
                //set file to open
                filestat = OPEN;

        }

    }

    //for all commands besides open, if the file is closed, print an error
    else if(filestat == CLOSED)
    {
        printf("Error: File system image not open.\n");
    }
    //closes the file
    else if(strcmp(token[0],"close")==0)
    {
        fclose(fp);
        filestat = CLOSED;
    }
    //calls the info command
    else if(strcmp(token[0],"info")==0)
    {
        info(fp);

    }





    free( working_root );

  }

  return 0;
}


//takes the file pointer as a parameter
//prints out the bytes per sector, sectors per cluster, Reserved sector count, number of fats
//and the fat size in decimal and hexadecimal for the fat32 file system image
void info(FILE *fp)
{
    int16_t BPB_BytsPerSec;
    int8_t BPB_SecPerClus;
    int16_t BPB_RsvdSecCnt;
    int8_t BPB_NumFats;
    int32_t BPB_FATSz32;

    //for each variable, sets fp pointer to the appropriate byte in the file according to the
    //spec, reads the file then prints the values
    fseek(fp, 11, SEEK_SET);
    fread(&BPB_BytsPerSec, sizeof(int16_t), 1,fp);
    printf("BPB_BytesPerSec: %d, %x\n",BPB_BytsPerSec, BPB_BytsPerSec);


    fseek(fp, 13, SEEK_SET);
    fread(&BPB_SecPerClus, sizeof(int8_t), 1, fp);
    printf("BPB_SecPerClus: %d, %x\n",BPB_SecPerClus, BPB_SecPerClus);

    fseek(fp,14, SEEK_SET );
    fread(&BPB_RsvdSecCnt, sizeof(int16_t), 1, fp);
    printf("BPB_RsvdSecCnt: %d, %x\n",BPB_RsvdSecCnt, BPB_RsvdSecCnt);

    fseek(fp, 16, SEEK_SET);
    fread(&BPB_NumFats, sizeof(int8_t), 1, fp);
    printf("BPB_NumFats: %d, %x\n",BPB_NumFats, BPB_NumFats);


    fseek(fp, 36, SEEK_SET);
    fread(&BPB_FATSz32, sizeof(int32_t), 1, fp);
    printf("BPB_FATSz32: %d, %x\n", BPB_FATSz32, BPB_FATSz32);






}

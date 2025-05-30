/*-------------------------------------------------------------------
File: filter.c

Description:  Quick and dirty code. In particular, gets() is not a 
              good function to use since it can write past the end
              of the memory buffer it receives.
              This program is specific to processing output received
	      from the procmon program.
---------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

int main(int argc, char **argv)
{
    char buffer[128], oldstate[32], state[32];
    int i;

    state[0] = oldstate[0] = 0;

    // Copy the first five lines from input to output
    for(i = 0; i < 5; i++) 
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            exit(-1); // Error reading header lines
        else
            fputs(buffer, stdout);
    }

    while(fgets(buffer, sizeof(buffer), stdin)) 
    {
        sscanf(buffer, "%*s %s", state);
        if (strcmp(state, oldstate))  
        {
            strcpy(oldstate, state);
            fputs(buffer, stdout);
            fflush(stdout);
        }
    }

    return 0;
}


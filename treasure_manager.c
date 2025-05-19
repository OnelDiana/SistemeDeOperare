#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>

#include "verificari.h"
#include "functii.h"
#include "optiuni.h"

int main(int argc, char** argv)
{
  if(argc < 2)
    {
      printf(rosu "Numarul de argumente este prea mic!" reset "\n" );
      exit(EXIT_FAILURE);
    }
  
  if(strcmp(argv[1],"--add") == 0)
    {
      verificaNrArg(argc, 3);
      adaugaTreasure(argv[2]);
    }

  else if(strcmp(argv[1],"--list") == 0)
    {
      verificaNrArg(argc, 3);
      listeazaHunt(argv[2]);
    }

  else if(strcmp(argv[1],"--view") == 0)
    {
      verificaNrArg(argc, 4);
      viewTreasure(argv[2], argv[3]);
    }

  else if(strcmp(argv[1],"--remove_treasure") == 0)
    {
      verificaNrArg(argc, 4);
      stergeTreasure(argv[2], argv[3]);
    }
  
  else if(strcmp(argv[1],"--remove_hunt") == 0)
    {
      verificaNrArg(argc, 3);
      stergeHunt(argv[2]);
    }

  else
    {
      printf(rosu "Optiunea introdusa nu este valida!" reset "\n");
      afiseazaOptiuni();
    }
  
  return 0;
}

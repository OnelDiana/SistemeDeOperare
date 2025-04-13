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

void adaugaTreasure(char *idH)
{
  DIR *dir = NULL;
  verificaDeschidereDirectorSiMKDIR(&dir, idH);
  
  int fd; //descriptor fisier
  char cale[50];

  snprintf(cale, sizeof(cale), "%s/treasures.dat", idH);
  fd = open(cale, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  verificaDeschidereFisier(fd, dir);

  TREASURE treasureDeAdaugat;
  citireInfoTreasure(&treasureDeAdaugat);

  if(write(fd, &treasureDeAdaugat, sizeof(TREASURE)) == -1)
    {
      perror("Eroare la scrierea in fisier! \n");
      close(fd);
      closedir(dir);
      exit(EXIT_FAILURE);
    }

  char actiune[100];
  snprintf(actiune, sizeof(actiune), "Am adaugat un treasure cu id-ul %s! \n", treasureDeAdaugat.treasureID);
  logActiune(idH, actiune);
    
  verificaInchidereFisier(fd);
  verificaInchidereDirector(dir);
}

void stergeTreasure(char *idH, char *idT)
{
  DIR *dir = opendir(idH);
  verificaDeschidereDirector(dir);

  char cale[50];
  snprintf(cale, sizeof(cale), "%s/treasures.dat", idH); //aici este treasure-ul ce trebuie sters

  char caleFisierNou[50];
  snprintf(caleFisierNou, sizeof(caleFisierNou), "%s/treasuresNou.dat", idH);

  int fd; 
  fd = open(cale, O_RDONLY);
  verificaDeschidereFisier(fd, dir);

  int fdNou;
  fdNou = open(caleFisierNou, O_WRONLY |  O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  verificaDeschidereFisier(fdNou, dir);

  TREASURE treasure;
  ssize_t n;

  while((n = read(fd, &treasure, sizeof(TREASURE))) != 0)
    {
      if(n == -1)
	{
	  perror("Eroare la citirea din fisier! \n");
	  exit(EXIT_FAILURE);
	}

      if(strcmp(treasure.treasureID, idT))
	{
	  if(write(fdNou, &treasure, sizeof(TREASURE)) == -1)
	    {
	      perror("Eroare la scrierea in fisier! \n");
	      exit(EXIT_FAILURE);
	    }
	}
    }

  verificaInchidereFisier(fd);
  verificaInchidereFisier(fdNou);
  
  remove(cale);
  rename(caleFisierNou, cale);
  verificaInchidereDirector(dir);

  char actiune[100];
  snprintf(actiune, sizeof(actiune), "Am sters treasure-ul cu id-ul %s din hunt-ul cu id-ul %s! \n", idT, idH);
  logActiune(idH, actiune);
}

void stergeHunt(char *idH)
{
  DIR *dir = opendir(idH);
  verificaDeschidereDirector(dir);

  char caleHunt[50];
  snprintf(caleHunt, sizeof(caleHunt), "%s", idH); 
  
  struct dirent *intrare;
 
  while((intrare = readdir(dir)) != NULL)
    {
      if((strcmp(intrare->d_name, ".") == 0) || (strcmp(intrare->d_name, "..") == 0))
	 continue;

      char cale[128];
      snprintf(cale, sizeof(cale), "%s/%s", idH, intrare->d_name);

      if(remove(cale) != 0)
	{
	  perror("Eroare la stergere! \n");
	  closedir(dir);
	  exit(EXIT_FAILURE);
	} 
    }
  
  verificaInchidereDirector(dir);
  
  if(rmdir(caleHunt) != 0)
    {
      perror("Eroare la stergerea directorului! \n");
      exit(EXIT_FAILURE);
    }

}

void listeazaHunt(char *idH)
{
  DIR *dir = opendir(idH);
  verificaDeschidereDirector(dir);

  infoHunt(idH);

  char cale[50];
  snprintf(cale, sizeof(cale), "%s/treasures.dat", idH);
  
  int fd; 
  fd = open(cale, O_RDONLY);
  verificaDeschidereFisier(fd, dir);
  
  TREASURE treasure;
  ssize_t n;

  struct stat info;
	if(stat(cale, &info) == -1)
	  {
	     perror("Eroare!\n");
	     exit(EXIT_FAILURE);
	  }
	printf("Dimensiunea fisierului %s in bytes: %lld\n", cale, info.st_size);
	printf("Ultima modificare a fisierului cu Treasure-urile: %ld\n\n", info.st_mtime);
	
  while((n = read(fd, &treasure, sizeof(TREASURE))) != 0)
    {
      if(n == -1)
	{
	  perror("Eroare la citirea din fisier! \n");
	  exit(EXIT_FAILURE);
	}
	  
	infoTreasure(treasure);	
    }
  
  verificaInchidereFisier(fd);
  
  char actiune[128];
  snprintf(actiune, sizeof(actiune), "Am listat hunt-ul cu id-ul %s! \n", idH);
  logActiune(idH, actiune);

  verificaInchidereDirector(dir);
}

 
void viewTreasure(char *idH, char *idT)
{
  DIR *dir = opendir(idH);
  verificaDeschidereDirector(dir);

  char cale[50];
  snprintf(cale, sizeof(cale), "%s/treasures.dat", idH);
  
  int fd; 
  fd = open(cale, O_RDONLY);
  verificaDeschidereFisier(fd, dir);
  
  TREASURE treasure;
  ssize_t n;

  while((n = read(fd, &treasure, sizeof(TREASURE))) == sizeof(TREASURE))
    {
      if(strcmp(treasure.treasureID, idT) == 0)
	{
	  infoTreasure(treasure);
	}
    }
  
  verificaInchidereFisier(fd);

  char actiune[128];
  snprintf(actiune, sizeof(actiune), "Am listat informatiile despre treasure-ul cu id-ul %s din hunt-ul cu id-ul  %s! \n", idT, idH);
  logActiune(idH, actiune);

  verificaInchidereDirector(dir);
}

      

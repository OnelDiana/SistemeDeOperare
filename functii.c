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

void citireInfoTreasure(TREASURE *treasure)
{
  printf("Introduceti informatiile necesare despre Treasure:\n\n");
  
  printf("Treasure ID: "); 
  scanf("%s", treasure->treasureID);
  verificaSirValid(treasure->treasureID);
  
  printf("User Name: ");
  scanf("%s", treasure->userName);
  verificaSirValid(treasure->userName);
  
  printf("Clue Text: ");
  scanf("%s", treasure->clueText);
  verificaSirValid(treasure->clueText);
  
  printf("Value: ");
  scanf("%d", &treasure->value);
  verificaValuePozitiv(treasure->value);
  
  printf("GPS Coordinates:\n");
  
  printf("Latitude: ");
  scanf("%f", &treasure->latitude);
  verificaGPSlatitude(treasure->latitude);
  
  printf("Longitude: ");
  scanf("%f", &treasure->longitude);
  verificaGPSlongitude(treasure->longitude);
  
  printf("\n\n");
}
 
void infoTreasure(TREASURE treasure)
{
  printf("Treasure ID: %s\n", treasure.treasureID);
  printf("Treasure Nume Utilizator: %s\n", treasure.userName);
  printf("Treasure Indiciu: %s\n", treasure.clueText);
  printf("Treasure Valoare: %d\n", treasure.value);
  printf("Treasure Coordonate GPS: ");
  if(treasure.latitude > 0)
    printf("Latitudine Emisfera Nordica: %f - ", treasure.latitude);
  if(treasure.latitude < 0)
    printf("Latitudine Emisfera Sudica: %f - ", treasure.latitude);
  if(treasure.longitude > 0)
    printf("Longitude Emisfera Estica: %f\n", treasure.longitude);
  if(treasure.longitude < 0)
    printf("Longitude Emisfera Vestica: %f\n", treasure.longitude);
  printf("\n\n");
}

off_t dimensiuneHunt(char *idH)
{
  DIR *dir = opendir(idH);
  verificaDeschidereDirector(dir);

  struct dirent *intrare;
  struct stat info;
  char cale[128];
  off_t total = 0;

  while((intrare = readdir(dir)) != NULL)
    {
      if((strcmp(intrare->d_name, ".") == 0) || (strcmp(intrare->d_name, "..") == 0))
	 continue;

      snprintf(cale, sizeof(cale), "%s/%s", idH, intrare->d_name);

      if(stat(cale, &info) == 0)
	{
	  total = total + info.st_size;
	}
    }
  
  verificaInchidereDirector(dir);
  
  return total;
}

time_t ultimaModificare(char *idH)
{
  DIR *dir = opendir(idH);
  verificaDeschidereDirector(dir);

  struct dirent *intrare;
  struct stat info;
  char cale[128];
  time_t timp = 0;

  while((intrare = readdir(dir)) != NULL)
    {
      if((strcmp(intrare->d_name, ".") == 0) || (strcmp(intrare->d_name, "..") == 0))
	 continue;

      snprintf(cale, sizeof(cale), "%s/%s", idH, intrare->d_name);

      if(stat(cale, &info) == 0)
	if(info.st_mtime > timp)
	  {
	    timp = info.st_mtime;
	  }
    }
  
  verificaInchidereDirector(dir);
  
  return timp;
}

void infoHunt(char *idH)
{
  printf("Numele Hunt-ului: %s\n", idH);

  off_t dimHunt = dimensiuneHunt(idH);
  printf("Dimensiunea totala in bytes a Hunt-ului:  %llu\n", dimHunt);

  time_t ultimaModifHunt = ultimaModificare(idH);
  char *timp = ctime(&ultimaModifHunt);
  printf("Ultima modificare a Hunt-ului: %s\n", timp); 
}

void logActiune(char *idH, char *actiune)
{
  char fisierLogCale[128];
  snprintf(fisierLogCale, sizeof(fisierLogCale), "%s/logged_hunt.txt", idH);
  
  int fl; //descriptor fisier de log
  fl = open(fisierLogCale, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  if(fl == -1)
    {
      perror("Eroare la deschiderea fisierului de log! \n");
      exit(EXIT_FAILURE);
     }
	     
  if(write(fl, actiune, strlen(actiune)) == -1)
    {
      perror("Eroare la scrierea in fisierul de log! \n");
      close(fl);
      exit(EXIT_FAILURE);
    }
  
  verificaInchidereFisier(fl);
}

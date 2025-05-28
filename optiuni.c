#include "biblioteci.h"

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
      perror(rosu "Eroare la scrierea in fisier!" reset "\n");
      close(fd);
      closedir(dir);
      exit(EXIT_FAILURE);
    }

  char actiune[128];
  snprintf(actiune, sizeof(actiune), "Am adaugat un treasure cu id-ul %s! \n", treasureDeAdaugat.treasureID);
  logActiune(idH, actiune);
  logSymlink(idH);
    
  verificaInchidereFisier(fd);
  verificaInchidereDirector(dir);
}

void stergeTreasure(char *idH, char *idT)
{
  printf(verde "Optiunea introdusa urmeaza sa stearga treasure-ul cu id-ul %s din hunt-ul %s!" reset "\n", idT, idH);
  printf("Sunteti sigur? Daca doriti sa continuati introduceti " blue "DA" reset ": ");

  int ok = mesajStergere();

  if(ok == 1)
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
      int gasit = 0;

      while((n = read(fd, &treasure, sizeof(TREASURE))) != 0)
	{
	  if(n == -1)
	    {
	      perror(rosu "Eroare la citirea din fisier!" reset "\n");
	      exit(EXIT_FAILURE);
	    }

	  if(strcmp(treasure.treasureID, idT) != 0)
	    {
	      if(write(fdNou, &treasure, sizeof(TREASURE)) == -1)
		{
		  perror(rosu "Eroare la scrierea in fisier!" reset "\n");
		  exit(EXIT_FAILURE);
		}
	    }
	  else
	    {
	      gasit = 1;
	    }
	}

      verificaInchidereFisier(fd);
      verificaInchidereFisier(fdNou);

      if(gasit == 0)
	{
	  printf(blue "Treasure-ul cu id-ul %s nu exista!" reset "\n", idT);
	  remove(caleFisierNou);

	  char actiune[100];
	  snprintf(actiune, sizeof(actiune), "Am incercat stergerea treasure-ul cu id-ul %s din hunt-ul %s - nu exista \n", idT, idH);
	  logActiune(idH, actiune);
	  logSymlink(idH);
	}
      else
	{
	  remove(cale);
	  rename(caleFisierNou, cale);

	  char actiune[100];
	  snprintf(actiune, sizeof(actiune), "Am sters treasure-ul cu id-ul %s din hunt-ul %s! \n", idT, idH);
	  logActiune(idH, actiune);
	  logSymlink(idH);
	}

      verificaInchidereDirector(dir);
    }
  else
    {
      printf(verde "Treasure-ul nu va fi sters!" reset "\n");
    }
}

void stergeHunt(char *idH)
{
  printf(verde "Optiunea introdusa urmeaza sa stearga hunt-ul %s!" reset "\n", idH);
  printf("Sunteti sigur? Daca doriti sa continuati introduceti " blue "DA" reset ": ");
  
  int ok = mesajStergere();

  if(ok == 1)
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
	      perror(rosu "Eroare la stergere!" reset "\n");
	      closedir(dir);
	      exit(EXIT_FAILURE);
	    } 
	}
  
      verificaInchidereDirector(dir);
  
      if(rmdir(caleHunt) != 0)
	{
	  perror(rosu "Eroare la stergerea directorului!" reset "\n");
	  exit(EXIT_FAILURE);
	}

      char link[128];
      snprintf(link, sizeof(link), "logged_hunt.txt-%s", idH);
      if(unlink(link) == -1)
	{
	  perror(rosu "Eroare unlink!" reset "\n");
	  exit(EXIT_FAILURE);
	}
    }
  else
    {
      printf(verde "Hunt-ul nu va fi sters!" reset "\n");
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
       perror(rosu "Eroare!" reset "\n");
       exit(EXIT_FAILURE);
    }

  char *timp = ctime(&info.st_mtime);
  
  printf("Dimensiunea fisierului %s in bytes:" blue " %lld" reset "\n", cale, info.st_size);
  printf("Ultima modificare a fisierului cu Treasure-urile:" blue "  %s" reset "\n\n", timp);
	
  while((n = read(fd, &treasure, sizeof(TREASURE))) != 0)
    {
      if(n == -1)
	{
	  perror(rosu "Eroare la citirea din fisier!" reset "\n");
	  exit(EXIT_FAILURE);
	}
	  
	infoTreasure(treasure);	
    }
  
  verificaInchidereFisier(fd);
  
  char actiune[128];
  snprintf(actiune, sizeof(actiune), "Am listat hunt-ul cu id-ul %s! \n", idH);
  logActiune(idH, actiune);
  logSymlink(idH);

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
  int gasit = 0;

  while((n = read(fd, &treasure, sizeof(TREASURE))) == sizeof(TREASURE))
    {
      if(strcmp(treasure.treasureID, idT) == 0)
	{
	  infoTreasure(treasure);

	  gasit = 1;
	  
	  char actiune[128];
	  snprintf(actiune, sizeof(actiune), "Am listat informatiile despre treasure-ul cu id-ul %s din hunt-ul %s! \n", idT, idH);
	  logActiune(idH, actiune);
	  logSymlink(idH);
	}
    }

  if(gasit == 0)
    {
      printf(blue "Treasure-ul cu id-ul %s nu exista!" reset "\n", idT);
      char actiune[128];
      snprintf(actiune, sizeof(actiune), "Am incercat listarea informatiilor despre treasure-ul cu id-ul %s din hunt-ul %s - nu exista \n", idT, idH);
      logActiune(idH, actiune);
      logSymlink(idH);
    }
  
  verificaInchidereFisier(fd);
  verificaInchidereDirector(dir);
}

      

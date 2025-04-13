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


void verificaNrArg(int argc, int nr)
{
  if(argc != nr)
    {
      printf("Numarul de argumente este gresit!\n");
      exit(EXIT_FAILURE);
    }
}

void verificaDeschidereFisier(int fd, DIR *dir)
{
  if(fd == -1)
    {
      perror("Eroare la deschiderea fisierului! \n");
      close(fd);
      closedir(dir);
      exit(EXIT_FAILURE);
    }
}

void verificaInchidereFisier(int fd)
{
  if(close(fd) == -1)
    {
      perror("Eroare la inchiderea fisierului! \n");
      exit(EXIT_FAILURE);
    }
}

void verificaDeschidereDirector(DIR *dir)
{
  if(dir == NULL)
    {
      perror("Eroare la deschiderea directorului! \n");
      closedir(dir);
      exit(EXIT_FAILURE);
    }
}

void verificaDeschidereDirectorSiMKDIR(DIR **dir, char *idH)
{
  *dir = opendir(idH);
  if(*dir == NULL) 
    {
      if(errno == ENOENT) //nu exista deja
	{
	  printf("Acest Hunt nu exista! Il vom crea! \n");
	  if(mkdir(idH, 0755) == -1) //nu a mers sa il cream
	    {
	      perror("Eroare la creare! \n");
	      closedir(*dir);
	      exit(EXIT_FAILURE);
	    }
	  else //l-am creat
	    {
	      *dir = opendir(idH); //il deschidem iar
	      if(*dir == NULL)
		{
		  perror("Eroare la deschiderea directorului! Acesta a fost creat! \n");
		  closedir(*dir);
		  exit(EXIT_FAILURE);
		}
	    }
	}
      else //exista 
	{
	  perror("Eroare la deschiderea directorului! \n");
	  closedir(*dir);
	  exit(EXIT_FAILURE);
	}
    }
}

void verificaInchidereDirector(DIR *dir)
{
  if(closedir(dir) == -1)
    {
      perror("Eroare la inchiderea directorului! \n");
      exit(EXIT_FAILURE);
    }
}

void verificaSirValid(char *sir)
{
  if((sir == NULL) || (strlen(sir) == 0))
    {
      printf("Sirul de caractere introdus nu este valid! \n");
      exit(EXIT_FAILURE);
    }
}

void verificaValuePozitiv(int value)
{
  if(value < 0)
    {
      printf("Valoarea introdusa nu este valida! Trebuie introdus un numar pozitiv! \n");
      exit(EXIT_FAILURE);
    }
}

void verificaGPSlatitude(float latitude)
{
  /*
    Latitudinea are valori între -90 și +90
    Pozitiv (+) = emisfera nordică (N)
    Negativ (−) = emisfera sudică (S)
  */

  if((latitude < -90) || (latitude > 90))
    {
      printf("Latitudinea introdusa nu este valida! \n");
      exit(EXIT_FAILURE);
    }
}

void verificaGPSlongitude(float longitude)
{
  /*
    Longitudinea are valori între -180 și +180
    Pozitiv (+) = emisfera estică (E)
    Negativ (−) = emisfera vestică (W)
   */

  if((longitude < -180) || (longitude > 180))
    {
      printf("Longitudinea introdusa nu este valida! \n");
      exit(EXIT_FAILURE);
    } 
}

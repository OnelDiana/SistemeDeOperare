#include "biblioteci.h"
#include "verificari.h"


void verificaNrArg(int argc, int nr)
{
  if(argc != nr)
    {
      printf(rosu "Numarul de argumente este gresit!" reset "\n");
      exit(EXIT_FAILURE);
    }
}

void verificaDeschidereFisier(int fd, DIR *dir)
{
  if(fd == -1)
    {
      perror(rosu "Eroare la deschiderea fisierului!" reset "\n");
      close(fd);
      closedir(dir);
      exit(EXIT_FAILURE);
    }
}

void deschidereFisier(int fd)
{
  if(fd == -1)
    {
      perror(rosu "Eroare la deschiderea fisierului!" reset  "\n");
      close(fd);
      exit(EXIT_FAILURE);
    }
}

void verificaInchidereFisier(int fd)
{
  if(close(fd) == -1)
    {
      perror(rosu "Eroare la inchiderea fisierului!" reset "\n");
      exit(EXIT_FAILURE);
    }
}

void verificaDeschidereDirector(DIR *dir)
{
  if(dir == NULL) 
    {
      if(errno == ENOENT) //nu exista
	{
	  printf(rosu "Acest Hunt nu exista!" reset "\n");
	  closedir(dir);
	  exit(EXIT_FAILURE);
	}
      else //alta eroare
	{
	  perror(rosu "Eroare la deschiderea directorului!" reset "\n");
	  closedir(dir);
	  exit(EXIT_FAILURE);
	}
    }
}

void verificaDeschidereDirectorSiMKDIR(DIR **dir, char *idH)
{
  *dir = opendir(idH);
  if(*dir == NULL) 
    {
      if(errno == ENOENT) //nu exista deja
	{
	  printf(rosu "Acest Hunt nu exista!" reset blue "Il vom crea!" reset "\n");
	  if(mkdir(idH, 0755) == -1) //nu a mers sa il cream
	    {
	      perror(rosu "Eroare la creare!" reset "\n");
	      closedir(*dir);
	      exit(EXIT_FAILURE);
	    }
	  else //l-am creat
	    {
	      *dir = opendir(idH); //il deschidem iar
	      if(*dir == NULL)
		{
		  perror(verde "Directorul a fost creat!" reset rosu "Eroare la deschidere!" reset "\n");
		  closedir(*dir);
		  exit(EXIT_FAILURE);
		}
	    }
	}
      else //exista 
	{
	  perror(rosu "Eroare la deschiderea directorului!" reset "\n");
	  closedir(*dir);
	  exit(EXIT_FAILURE);
	}
    }
}

void verificaInchidereDirector(DIR *dir)
{
  if(closedir(dir) == -1)
    {
      perror(rosu "Eroare la inchiderea directorului!" reset "\n");
      exit(EXIT_FAILURE);
    }
}

void verificaSirValid(char *sir)
{
  if((sir == NULL) || (strlen(sir) == 0))
    {
      printf(rosu "Sirul de caractere introdus nu este valid!" reset "\n");
      exit(EXIT_FAILURE);
    }
}

void verificaValuePozitiv(int value)
{
  if(value < 0)
    {
      printf(rosu "Valoarea introdusa nu este valida! Trebuie introdus un numar pozitiv!" reset "\n");
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
      printf(rosu "Latitudinea introdusa nu este valida!" reset "\n");
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
      printf(rosu "Longitudinea introdusa nu este valida!" reset "\n");
      exit(EXIT_FAILURE);
    } 
}

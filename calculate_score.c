#include "biblioteci.h"

#include "verificari.h"

typedef struct{

  char treasureID[20];
  char userName[32];
  char clueText[128];
  int value;
  float latitude, longitude;

}TREASURE;

typedef struct SCOR{

  char nume[32];
  int scor;
  struct SCOR *urm;
  
}SCOR;

SCOR *adaugareUtilizatorNou(SCOR **head, char *nume)
{
  SCOR *curent = *head;

  while(curent)
    {
      if(strcmp(curent->nume, nume) == 0)
	{
	  return curent;
	}
      curent = curent->urm;
    }

  SCOR *nou = malloc(sizeof(SCOR));

  strncpy(nou->nume, nume, 32);
  nou->scor = 0;
  nou->urm = *head;
  *head = nou;

  return nou;
}

void elibereaza(SCOR *head)
{
  while(head)
    {
      SCOR *aux = head;
      head = head->urm;
      free(aux);
    }
}

int main(int argc, char *argv[])
{
  verificaNrArg(argc, 2);

  char cale[256];
  snprintf(cale, sizeof(cale), "%s/treasures.dat", argv[1]);
  
  int fd;
  fd = open(cale, O_RDONLY);
  deschidereFisier(fd);

  TREASURE treasure;
  SCOR *scoruri = NULL;

  while(read(fd, &treasure, sizeof(TREASURE)) == sizeof(TREASURE))
    {
      SCOR *user = adaugareUtilizatorNou(&scoruri, treasure.userName);
      user->scor = user->scor + treasure.value;
    }

  if(scoruri == NULL)
    {
      printf(blue "Nu au fost gasite treasure-uri!" reset "\n");
      fflush(stdout);
    }

  SCOR *curent = scoruri;
  while(curent)
    {
      printf(blue "%s" reset ":" verde " %d" reset "\n\n", curent->nume, curent->scor);
      fflush(stdout);
      curent = curent->urm;
    }

  elibereaza(scoruri);
  fflush(stdout);

  verificaInchidereFisier(fd);
  
  return 0;
}

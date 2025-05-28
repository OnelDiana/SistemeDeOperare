#include "biblioteci.h"

#include "verificari.h"
#include "functii.h"

void afiseazaOptiuni()
{
  printf("\n");

  printf(verde "Optiunile disponibile: " reset "\n");
  printf(blue "--add " reset "id_hunt \n");
  printf(blue "--list " reset "id_hunt \n");
  printf(blue "--view " reset "id_hunt id_treasure \n");
  printf(blue "--remove_treasure " reset "id_hunt id_treasure \n");
  printf(blue "--remove_hunt " reset "id_hunt \n\n");
}

void citireInfoTreasure(TREASURE *treasure)
{
  printf(verde "Introduceti informatiile necesare despre Treasure:" reset "\n\n");
  
  printf(blue "Treasure ID: " reset); 
  scanf("%s", treasure->treasureID);
  getchar();
  verificaSirValid(treasure->treasureID);
  
  printf(blue "User Name: " reset);
  fgets(treasure->userName, 64, stdin);
  treasure->userName[strcspn(treasure->userName,"\n")] = '\0';
  verificaSirValid(treasure->userName);
  
  printf(blue "Clue Text: " reset);
  fgets(treasure->clueText, 128, stdin);
  treasure->clueText[strcspn(treasure->clueText,"\n")] = '\0';
  verificaSirValid(treasure->clueText);
  
  printf(blue "Value: " reset);
  scanf("%d", &treasure->value);
  verificaValuePozitiv(treasure->value);
  
  printf(blue "GPS Coordinates:" reset "\n");
  
  printf(blue "Latitude: " reset);
  scanf("%f", &treasure->latitude);
  verificaGPSlatitude(treasure->latitude);
  
  printf(blue "Longitude: " reset);
  scanf("%f", &treasure->longitude);
  verificaGPSlongitude(treasure->longitude);
  
  printf("\n\n");
}
 
void infoTreasure(TREASURE treasure)
{
  printf("\n");
  printf("Treasure ID: " verde "%s" reset "\n", treasure.treasureID);
  printf("Treasure Nume Utilizator: " verde "%s" reset "\n", treasure.userName);
  printf("Treasure Indiciu: " verde  "%s" reset "\n", treasure.clueText);
  printf("Treasure Valoare: " verde "%d" reset "\n", treasure.value);
  printf("Treasure Coordonate GPS: ");
  if(treasure.latitude > 0)
    printf("Latitudine Emisfera Nordica: " verde "%.2f" reset " - ", treasure.latitude);
  if(treasure.latitude < 0)
    printf("Latitudine Emisfera Sudica: " verde "%.2f" reset " - ", treasure.latitude);
  if(treasure.longitude > 0)
    printf("Longitude Emisfera Estica: " verde "%.2f" reset "\n", treasure.longitude);
  if(treasure.longitude < 0)
    printf("Longitude Emisfera Vestica: " verde "%.2f" reset "\n", treasure.longitude);
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
  printf(verde "Numele Hunt-ului: %s" reset "\n\n", idH);

  off_t dimHunt = dimensiuneHunt(idH);
  printf("Dimensiunea totala in bytes a Hunt-ului:" blue "  %llu" reset "\n", dimHunt);

  time_t ultimaModifHunt = ultimaModificare(idH);
  char *timp = ctime(&ultimaModifHunt);
  printf("Ultima modificare a Hunt-ului:" blue "  %s" reset "\n", timp); 
}

void logActiune(char *idH, char *actiune)
{
  char fisierLogCale[128];
  snprintf(fisierLogCale, sizeof(fisierLogCale), "%s/logged_hunt.txt", idH);
  
  int fl; //descriptor fisier de log
  fl = open(fisierLogCale, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  deschidereFisier(fl);
	     
  if(write(fl, actiune, strlen(actiune)) == -1)
    {
      perror(rosu "Eroare la scrierea in fisierul de log!" reset "\n");
      close(fl);
      exit(EXIT_FAILURE);
    }
  
  verificaInchidereFisier(fl);
}

void logSymlink(char *idH)
{
  char cale1[128], cale2[128];

  snprintf(cale1, sizeof(cale1), "%s/logged_hunt.txt", idH);
  snprintf(cale2, sizeof(cale2), "logged_hunt.txt-%s", idH);

  unlink(cale2);

  if(symlink(cale1, cale2) == -1)
    {
      perror(rosu "Eroare symlink!" reset "\n");
      exit(EXIT_FAILURE);
    }
}

int mesajStergere()
{
  char rasp[4];
  scanf("%3s", rasp);

  if(strcasecmp(rasp, "DA") == 0)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

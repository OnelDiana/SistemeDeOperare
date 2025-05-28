#include "biblioteci.h"

#include "functii.h"
#include "verificari.h"

#define FIFO "/tmp/fifo"

void handle_term(int sig)
{
  printf("[Monitor]" verde " Semnalul de terminare a procesului monitor a fost primit! " reset);
  
  printf(blue "Terminarea va fi intarziata cu 5 secunde..." reset "\n");

  fflush(stdout);

  usleep(5000000);
  printf("[Monitor]" verde " Terminat." reset "\n");
  fflush(stdout);

  exit(0);
}

void handle_list_hunts(int sig)
{
  DIR *dir = opendir(".");
  verificaDeschidereDirector(dir);

  struct dirent *intrare;

   while((intrare = readdir(dir)) != NULL)
	{
	  if((strcmp(intrare->d_name, ".") == 0) || (strcmp(intrare->d_name, "..") == 0))
	    continue;
	  
	  if(intrare->d_type != DT_DIR)
	    continue;

	  char cale[128];
	  snprintf(cale, sizeof(cale), "%s/treasures.dat", intrare->d_name);

	  int fd; 
	  fd = open(cale, O_RDONLY);
	  if(fd == -1)
	    {
	      continue;
	    }

	  int nrTreasures = 0;
	  TREASURE treasure;

	  while(read(fd, &treasure, sizeof(treasure)) == sizeof(treasure))
	    {
	      nrTreasures++;
	    }

	  verificaInchidereFisier(fd);

	  printf("Hunt:" verde " %s" reset " - Numarul de treasure-uri din hunt:" verde  " %d" reset "\n\n", intrare->d_name, nrTreasures);
	  fflush(stdout);
	}
   
   verificaInchidereDirector(dir);
}

void handle_list_view(int sig)
{
  int fd;
  fd = open(FIFO, O_RDONLY);
  deschidereFisier(fd);

  char comanda[128];

  if(read(fd, comanda, sizeof(comanda)) > 0)
    {
      comanda[strcspn(comanda, "\n")] = '\0';

      char *idH = strtok(comanda, " ");
      char *idT = strtok(NULL, " ");

      if(idH == NULL)
	{
	  verificaInchidereFisier(fd);
	  return;
	}

      char *args[5];
      args[0] = "./treasure_manager";
      
      if(idT)
	{
	  args[1] = "--view";
	  args[2] = idH;
	  args[3] = idT;
	  args[4] = NULL;
	}

      else
	{
	  args[1] = "--list";
	  args[2] = idH;
	  args[3] = NULL;
	}

      pid_t pid = fork();

      if(pid == 0)
	{
	  execv("./treasure_manager", args);

	  perror(rosu "Eroare execv!" reset "\n");
	  exit(EXIT_FAILURE);
	}

      if(pid > 0)
	{
	  waitpid(pid, NULL, 0);
	}

      if(pid == -1)
	{
	  perror(rosu "Eroare fork!" reset "\n");
	}
    }
  else
    {
      perror(rosu "Eroare la citirea din FIFO!" reset "\n");
    }

  verificaInchidereFisier(fd);
}

void setup_signal(int sig, void (*handler)(int))
{
  struct sigaction sa;
  sa.sa_handler = handler;

  if(sigaction(sig, &sa, NULL) == -1)
    {
      perror(rosu "Eroare sigaction!" reset "\n");
      exit(EXIT_FAILURE);
    }
}

int main()
{
  setbuf(stdout, NULL);

  printf("[Monitor]" blue " PID" reset ":" verde " %d" reset "\n\n", getpid());
  fflush(stdout);

  setup_signal(SIGUSR1, handle_list_hunts);
  setup_signal(SIGUSR2, handle_list_view);
  setup_signal(SIGTERM, handle_term);

  while(1)
    {
      pause();     
    }
  
  return 0;
} 

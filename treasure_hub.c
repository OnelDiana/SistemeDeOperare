#include "biblioteci.h"

#include "verificari.h"
#include "functii.h"

#define FIFO "/tmp/fifo"

pid_t monitorPid = -1;
pid_t pid = -1;

int pfd[2]; //pfd[0] - citire / pfd[1] - scriere

char opt[128];

typedef struct{

  char userName[64];
  int scorTotal;
  
}SCOR;

SCOR scorUtilizatori[100];
int nrUtilizatori = 0;

void afiseazaOptiuni()
{
  printf("\n" verde "Introduceti una din optiunile disponibile:" reset "\n");
  printf("-> " blue "start_monitor" reset "\n-> " blue "stop_monitor" reset "\n-> " blue "exit" reset "\n");
  printf("-> " blue "list_hunts" reset "\n-> " blue "list_treasures" reset "\n-> " blue "view_treasure" reset "\n");
  printf("-> " blue  "calculate_scores" reset "\n\n");

  fflush(stdout);
}

void startMonitor()
{
  if(monitorPid != -1)
    {
      printf(verde "Monitorul deja ruleaza!" reset "\n");
      return;
    }

  if(pipe(pfd) == -1)
    {
      perror(rosu "Eroare pipe!" reset "\n");
      exit(EXIT_FAILURE);
    }

  monitorPid = fork();
  
  if(monitorPid < 0)
    {
      perror(rosu "Eroare fork!" reset "\n");
      exit(EXIT_FAILURE);
    }

  if(monitorPid == 0) //codul fiului
    {
      close(pfd[0]);

      dup2(pfd[1], STDOUT_FILENO);
      dup2(pfd[1], STDERR_FILENO);

      close(pfd[1]);
      
      execlp("./monitor", "monitor", NULL);
  
      //daca am ajuns aici a esuat
      perror(rosu "Eroare execlp!" reset "\n");
      exit(EXIT_FAILURE);
    }
  
  close(pfd[1]);

  pid = fork();
  if(pid == 0)
    {
      char temp[1024];
      ssize_t n;
      
      while((n = read(pfd[0], temp, sizeof(temp) - 1)) > 0)
	{
	  temp[n] = '\0';
	  printf("\n %s", temp);
	  fflush(stdout);
	}

      close(pfd[0]);
      exit(0);	
    }

}

void stopMonitor()
{
  if(monitorPid == -1)
    {
      printf(verde "Monitorul nu ruleaza!" reset "\n");
      return;
    }

  kill(monitorPid, SIGTERM);
  
  int status;
  waitpid(monitorPid, &status, 0);
  
  printf("Monitorul a fost oprit cu " blue "statusul" reset ":" verde " %d" reset " - stop_monitor \n\n", status);
  monitorPid = -1;

  kill(pid, SIGTERM);
  waitpid(pid, NULL, 0);
}

void calculeazaScor()
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

	  struct stat st;
	  
	  char cale[128];
	  snprintf(cale, sizeof(cale), "%s/treasures.dat", intrare->d_name);

	  int fd; 
	  fd = open(cale, O_RDONLY);
	  if(fd == -1)
	    {
	      continue;
	    }

	  if(stat(cale, &st) == -1)
	    {
	      perror(rosu "Eroare stat!" reset "\n");
	      return;
	    }
	  
	  int pfds[2];
          pipe(pfds);
	  
          pid_t pid = fork();

	  if(pid == 0)
	    {
	      close(pfds[0]);
	      dup2(pfds[1],STDOUT_FILENO);
	      close(pfds[1]);
	      
	      execl("./calculate_score", "calculate_score", intrare->d_name, NULL);
	      
	      perror(rosu "Eroare execl!" reset "\n");
	      exit(EXIT_FAILURE);
	    }
	  
	  else 
	    {
	      close(pfds[1]);
	      
	      char temp[4096];
	      
	      printf("\nScorul pentru %s: \n", intrare->d_name);
	      fflush(stdout);
	      
	      int n;
	      while ((n = read(pfds[0], temp, sizeof(temp) - 1)) > 0) 
		{
		  temp[n] = '\0';
		  printf("%s", temp);
		  fflush(stdout);
		}
	      
	      waitpid(pid, NULL, 0);  
	      close(pfds[0]);         
	    }
	}
   
   verificaInchidereDirector(dir);
}

void trimiteSemnal(int sig)
{
  if(monitorPid == -1)
    {
      printf(verde "Monitorul nu ruleaza!" reset "\n");
      printf("Optiune sugerata:" blue " start_monitor" reset "\n");
      return;
    }

  char comanda[256];

  if(sig == SIGUSR2)
    {
      if(strcmp(opt, "list_treasures") == 0)
	{
	  printf(blue "Numele hunt-ului: " reset);
	  scanf("%s", comanda);
	  getchar();
	}
      else if(strcmp(opt, "view_treasure") == 0)
	{
	  char idH[128], idT[128];
	  
	  printf(blue "Numele hunt-ului: " reset);
	  scanf("%s", idH);
	  getchar();

	  printf(blue "Numele treasure-ului: " reset);
	  scanf("%s", idT);
	  getchar();

	  snprintf(comanda, sizeof(comanda), "%s %s", idH, idT);
	}
    }

  if(kill(monitorPid, sig) == -1)
    {
      perror(rosu "Eroare kill!" reset "\n");
      return;
    }

  int fd = open(FIFO, O_WRONLY);
  deschidereFisier(fd);

  if(write(fd, comanda, strlen(comanda) + 1) == -1)
    {
      perror(rosu "Eroare scriere FIFO!" reset "\n");
    }

  verificaInchidereFisier(fd);
}

int main()
{
  if(mkfifo(FIFO, 0666) == -1 && errno != EEXIST)
    {
      perror(rosu "Eroare mkfifo!" reset "\n");
    }
  
  char optiune[128];
  
  afiseazaOptiuni();

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  while(1)
  {
    fflush(stdout);
    
    if(!fgets(optiune, sizeof(optiune), stdin))
      break;
    optiune[strcspn(optiune, "\n")] = 0;

    strncpy(opt, optiune, sizeof(opt));
    
    if(strcmp(optiune, "start_monitor") == 0)
      {
	startMonitor();
      }

    else if(strcmp(optiune, "stop_monitor") == 0)
      {
	 stopMonitor();
      }

    else if(strcmp(optiune, "list_hunts") == 0)
      {
	if(kill(monitorPid, SIGUSR1) == -1)
	  {
	    perror(rosu "Eroare kill!" reset "\n");
	  }
      }

    else if(strcmp(optiune, "list_treasures") == 0)
      {
	trimiteSemnal(SIGUSR2);
      }
  
    else if(strcmp(optiune, "view_treasure") == 0)
      {
	trimiteSemnal(SIGUSR2);
      }
  
    else if(strcmp(optiune, "exit") == 0)
      {
	if(monitorPid > 0)
	  {
	    printf(rosu "Eroare - monitorul inca ruleaza! Trebuie oprit inainte!" reset "\n");
	    fflush(stdout);
	  }
	else
	  {
	    printf(verde "..." reset "\n\n");
	    break;
	  }
      }

    else if(strcmp(optiune, "calculate_scores") == 0)
      {
	if(monitorPid > 0)
	  {
	    printf(rosu "Eroare - monitorul inca ruleaza! Trebuie oprit inainte!" reset "\n");
	    printf("Optiune sugerata: " blue "stop_monitor" reset "\n");
	    fflush(stdout);
	  }
	else
	  {
	    calculeazaScor();
	  }
      }
      
    else
      {
	printf(rosu "Optiunea introdusa nu este valida!" reset "\n");
	afiseazaOptiuni();
      }
  }

  unlink(FIFO);
  
  return 0;
}

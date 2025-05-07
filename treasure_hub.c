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

#include <sys/wait.h>
#include <signal.h>

#define file ".file"

pid_t monitorPid = -1;


void afiseazaComenzi()
{
  printf("Introduceti una din comenzile disponibile:\n");
  printf("start_monitor\tstop_monitor\texit\n");
  printf("list_hunts\tlist_treasures\tview_treasures\n");
}

void verificaDeschidereFisier(int fd)
{
  if(fd == -1)
    {
      perror("Eroare la deschiderea fisierului! \n");
      close(fd);
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

void scriereComanda(char *comanda)
{
  int fd;
  fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  verificaDeschidereFisier(fd);

  if((write(fd, comanda, strlen(comanda)) == -1) || (write(fd, "\n", 1) == -1))
    {
      perror("Eroare la scrierea in fisier! \n");
      close(fd);
      exit(EXIT_FAILURE);
    }

  verificaInchidereFisier(fd);
}

void comandaMonitor(char *comanda)
{
  scriereComanda(comanda);

  if(kill(monitorPid, SIGUSR1) == -1)
    {
      perror("Eroare kill!\n");
      exit(EXIT_FAILURE);
    }
}

void start_monitor()
{
  if(monitorPid > 0)
    {
      printf("Monitor running!\n");
      return;
    }

  if((monitorPid = fork()) < 0)
    {
      perror("Eroare fork!\n");
      exit(EXIT_FAILURE);
    }

  if(monitorPid == 0) //codul fiului
    {
      execlp("./monitor", "monitor", NULL);
      //daca am ajuns aici a esuat
      perror("Eroare execlp!\n");
      exit(EXIT_FAILURE);
    }
}

void stop_monitor()
{
  if(monitorPid <= 0)
    {
      printf("Monitor not running!\n");
      return;
    }

  if(kill(monitorPid, SIGTERM) == -1)
    {
      perror("Eroare kill!\n");
      exit(EXIT_FAILURE);
    }

  int status;
  if(waitpid(monitorPid, &status, 0) == -1)
    {
      perror("Eroare waitpid!\n");
      exit(EXIT_FAILURE);
    }
  printf("Status: %d\n", status);
  
  monitorPid = -1;
}

void handler(int sig)
{
  int status;
  if((waitpid(monitorPid, &status, 0)) == -1)
    {
      perror("Eroare waitpid!\n");
      exit(EXIT_FAILURE);
    }
  
  if(WIFEXITED(status))
    {
      printf("Monitor has exited with status %d!\n", WEXITSTATUS(status));
    }

  monitorPid = -1;
   
}

int main()
{
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  if((sigaction(SIGCHLD, &sa, NULL) == -1))
    {
      perror("Eroare sigaction!\n");
      exit(EXIT_FAILURE);
    }
    
  char comanda[256];

  afiseazaComenzi();
  
  while(1)
  {
  
    if(!fgets(comanda, sizeof(comanda), stdin))
      break;
    comanda[strcspn(comanda, "\n")] = 0;
    
    if(strcmp(comanda,"start_monitor") == 0)
      {
	start_monitor();
      }

    else if(strcmp(comanda,"stop_monitor") == 0)
      {
	stop_monitor();
      }

    else if(strcmp(comanda,"list_hunts") == 0)
      {
	comandaMonitor(comanda);
      }

    else if(strcmp(comanda,"list_treasures") == 0)
      {
	comandaMonitor(comanda);
      }
  
    else if(strcmp(comanda,"view_treasure") == 0)
      {
	comandaMonitor(comanda);
      }
  
    else if(strcmp(comanda,"exit") == 0)
      {
	exit(EXIT_SUCCESS);
      }
  
    else
      {
	printf("Optiunea introdusa nu este valida! \n");
      }
  
  }
  return 0;
}

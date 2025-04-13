#ifndef FUNCTII_H
#define FUNCTII_H

typedef struct{

  char treasureID[20];
  char userName[32];
  char clueText[128];
  int value;
  float latitude, longitude;

}TREASURE;

void citireInfoTreasure(TREASURE *treasure);

void infoTreasure(TREASURE treasure);

void infoHunt(char *idH);

void logActiune(char *idH, char *actiune);

#endif

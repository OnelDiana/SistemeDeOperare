#ifndef VERIFICARI_H
#define VERIFICARI_H

void verificaNrArg(int argc, int nr);

void verificaDeschidereFisier(int fd, DIR *dir);

void verificaInchidereFisier(int fd);

void verificaDeschidereDirector(DIR *dir);

void verificaDeschidereDirectorSiMKDIR(DIR **dir, char *idH);

void verificaInchidereDirector(DIR *dir);

void verificaSirValid(char *sir);

void verificaValuePozitiv(int value);

void verificaGPSlatitude(float latitude);

void verificaGPSlongitude(float longitude);

#endif

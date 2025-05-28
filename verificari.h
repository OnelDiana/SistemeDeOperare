#ifndef VERIFICARI_H
#define VERIFICARI_H

#define rosu "\033[31m"
#define verde "\033[32m"
#define blue "\033[34m"
#define reset "\033[0m"

void verificaNrArg(int argc, int nr);

void verificaDeschidereFisier(int fd, DIR *dir);

void deschidereFisier(int fd);

void verificaInchidereFisier(int fd);

void verificaDeschidereDirector(DIR *dir);

void verificaDeschidereDirectorSiMKDIR(DIR **dir, char *idH);

void verificaInchidereDirector(DIR *dir);

void verificaSirValid(char *sir);

void verificaValuePozitiv(int value);

void verificaGPSlatitude(float latitude);

void verificaGPSlongitude(float longitude);

#endif

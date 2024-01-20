#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_STATE 4 // Valores maiores para armazenar os espacos
#define MAX_CITY 50
#define MAX_COURSE 40

#define MAX_HEAP 3 //!!MUDAR PARA 19
#define MAX_INPUT_TAPES 3
#define MAX_OUTPUT_TAPES 1

#define NAMETXT "prova.txt"
#define NAMEBIN "data.bin"

#include <stdbool.h>

typedef struct {
  long mat;
  double grade;
  char state[MAX_STATE];
  char city[MAX_CITY];
  char course[MAX_COURSE];
} Register;


typedef struct {
  Register regs; // Registro armazenadpo
  bool isMarked; // Indicador se esta marcado com *
  short numTape; // Indicador a qual fita pertence
} ItemsHeap;

typedef struct {
  int nRegisters;
  int situation;
  bool P;
} Arguments;

#endif
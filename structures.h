#ifndef STRUCTURES_H
#define STRUCTURES_H

//Tamanho da estrutura
#define MAX_STATE 3 
#define MAX_CITY 51
#define MAX_COURSE 31

#define MAX_HEAP 19         // Tamanho memoria interna
#define MAX_INPUT_TAPES 19  // Quantidade fitas entrada
#define MAX_OUTPUT_TAPES 1  // Quantidade fitas saida
#define NAMEBIN "bin/data.bin" //Nome onde e salvo os dados em binario

#include <stdbool.h>

/* Estruturas Heap usada na geracao dos blocos ordenados (Selecao por substituicao)*/

typedef struct{
    long inscricao;
    double nota;
    char estado[MAX_STATE], cidade[MAX_CITY],curso[MAX_COURSE];
}tItem;

typedef struct{
    tItem item;
    int marcador;
    int numTape;
}tRegistro;

typedef struct {
    tRegistro* array;
    int tamanho;
    int capacidade;
} Heap;

/* Estruturas para intercalacao F + 1 */
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

#endif
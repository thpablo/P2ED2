#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include "structures.h"

Heap* criarHeap(int capacidade);


void desalocaHeap(Heap* heap);

void trocar(tRegistro* a, tRegistro* b) ;

void minHeapify(Heap* heap, int indice, int *comparacoes);

tRegistro extrairMinimo(Heap* heap, int *comparacoes);

void inserir(Heap* heap, tRegistro elemento, int *comparacoes);

int marcaRegistro(tRegistro,tRegistro);

void imprimirHeap(Heap* heap);

void desmarcaHeap(Heap* heap);


#endif
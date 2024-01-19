#include <stdio.h>
#include <stdlib.h>
#include "./structures.h"

/* Ler e converter arquivos */
void textToBinary(const char *txtFile, const char *binFile)
{
    FILE *input = fopen(txtFile, "r");
    FILE *output = fopen(binFile, "wb");

    if (input == NULL || output == NULL)
    {
        perror("Erro ao tentar abrir arquivo");
        exit(1);
    }

    Register reg;
    while (!feof(input))
    {
        fscanf(input, "%ld %lf", &reg.mat, &reg.grade);
        fgets(reg.state, MAX_STATE, input);
        fgets(reg.city, MAX_CITY, input);
        fgets(reg.course, MAX_COURSE, input);
        fwrite(&reg, sizeof(Register), 1, output);
    }

    fclose(input);
    fclose(output);
}

void readBinaryFile(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");

    if (file == NULL)
    {
        perror("Erro ao tentar abrir arquivo");
        exit(1);
    }
    short countBlock = 1;
    Register reg;
    printf("Bloco %d: \n", countBlock);
    while (fread(&reg, sizeof(Register), 1, file) == 1)
    {
        if(reg.mat == -1){
            countBlock++;
            printf("Bloco %d: \n", countBlock);
            continue;
        }
        // printf("Mat: %ld\n", reg.mat);
        printf("Grade: %.1f", reg.grade);
        /*
        printf("State: %s\n", reg.state);
        printf("City: %s\n", reg.city);
        printf("Course: %s\n", reg.course);*/
        printf("\n");
    }
    printf("\n");
    fclose(file);
}

/* Gerar Blocos Ordenados (Selecao por substituicao) */

// Funcao troca dois itens
void swap(ItemsHeap *a, ItemsHeap *b)
{
    ItemsHeap temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(ItemsHeap heap[], int n, int i)
{
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && ((heap[left].regs.grade < heap[smallest].regs.grade && !heap[left].isMarked) || heap[smallest].isMarked))
        smallest = left;

    if (right < n && ((heap[right].regs.grade < heap[smallest].regs.grade && !heap[right].isMarked) || heap[smallest].isMarked))
        smallest = right;

    if (smallest != i)
    {
        swap(&heap[i], &heap[smallest]);
        heapify(heap, n, smallest);
    }
}

void buildHeap(ItemsHeap heap[], int n)
{
    int startIdx = (n / 2) - 1;

    for (int i = startIdx; i >= 0; i--)
    {
        heapify(heap, n, i);
    }
}
void printHeap(ItemsHeap heap[], int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("Node %d: Grade: %lf\n",
               i + 1, heap[i].regs.grade);
    }
    printf("\n");
}

void generateOrderedBlocks(const char *dataFile) {
    FILE *data = fopen(dataFile, "r"); // Dados
    FILE *inputTapes[MAX_INPUT_TAPES]; // Fitas de entrada
    int indexTapes = 0;                // Index para fitas de entrada

    /* Registrador Invalido que indica mudanca de bloco */
    Register jumpBlockIndicator;
    jumpBlockIndicator.mat = -1; jumpBlockIndicator.grade = -1;

    /* Gera N fitas de entrada vazias */
    for (int i = 0; i < MAX_INPUT_TAPES; i++)
    {
        char nome[50];
        // Crie a string "input_tapeX.bin" substituindo X pelo valor de i
        sprintf(nome, "input_tape%d.bin", i + 1);
        inputTapes[i] = fopen(nome, "w+");
    }

    // Heap Memoria interna
    ItemsHeap heap[MAX_HEAP];

    // Contador de itens marcados
    int countMarkedItems = 0;

    // Preenche o heap com N itens
    for (int i = 0; i < MAX_HEAP; i++)
    {
        fread(&(heap[i].regs), sizeof(Register), 1, data);
        heap[i].isMarked = false;
    }

    /* Ordenar Heap */

    /* While enquanto != fim de arquivo */
    while (!feof(data))
    {
        /*printf("Antes de ordenar\n");
        printHeap(heap, MAX_HEAP);*/

        buildHeap(heap, MAX_HEAP); ////////////////////////////

        /*printf("Depois de Ordenar:\n");*/
        printHeap(heap, MAX_HEAP);

        // Escreve na fita o elemento na posicao 0 do heap
        fwrite(&(heap[0].regs), sizeof(Register), 1, inputTapes[indexTapes]); ///////////////
        // Salva nota que saiu para fazer verificacao da marcacao
        double gradeOut = heap[0].regs.grade;

        // Le registro em Dados e coloca na primeira posicao
        fread(&(heap[0].regs), sizeof(Register), 1, data); ///////////

        /* Determina se o elemento deve ser marcado (se for menor do que saiu) */
        heap[0].isMarked = (gradeOut > heap[0].regs.grade) ? 1 : 0; ///////////////////
        if (heap[0].isMarked)
        {
            printf("Marcou (saiu %lf entrou %lf)\n", gradeOut, heap[0].regs.grade);
            countMarkedItems++;
        }
        // Se todos os itens na memoria interna estiverem marcados
        if (countMarkedItems == MAX_HEAP)
        {
            fwrite(&(jumpBlockIndicator), sizeof(Register), 1, inputTapes[indexTapes]);
            // Muda Fita de Entrada que e armazenada
            indexTapes = (indexTapes + 1) % MAX_INPUT_TAPES;
            // Reseta marcador
            countMarkedItems = 0;
            // Retira marcacao
            for (int i = 0; i < MAX_HEAP; i++)
                heap[i].isMarked = false;
        }
        else
        {
            // Ordena heap
            /*printf("Antes de ordenar\n");
            printHeap(heap, MAX_HEAP);*/
            buildHeap(heap, MAX_HEAP); ////////////////
            /*printf("Depois de Ordenar:\n");*/
            printHeap(heap, MAX_HEAP);
        }
    }
    for (int i = 0; i < MAX_INPUT_TAPES; i++)
    {
        printf("Fita %d\n", i + 1);
        fclose(inputTapes[i]);
        char nome[50];
        sprintf(nome, "input_tape%d.bin", i + 1);
        readBinaryFile(nome);
    }
    fclose(data);
}


/* Intercalacao Balanceada F + 1 */

/*GERAR HEAP LENDO REGISTROS ODS PRIMEIROS BLOCOS DE CADA FITA*/
/* while(enquanto diferente de fim de arquivo){
    while (LER BLOCOS VAI ATE ACHAR MATRICULA = -1 OU FINAL DO ARQUIVO) 
}*/
/* Leio registro e coloco no heap pra construir */

int main()
{
    /* Ler e converter arquivo */
    textToBinary(NAMETXT, NAMEBIN);

    /* Gerar blocos ordenados */
    generateOrderedBlocks(NAMEBIN);

    return 0;
}

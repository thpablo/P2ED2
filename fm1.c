#include <stdio.h>
#include <stdlib.h>
#include "./structures.h"

/* Ler e converter arquivos */
void textToBinary(const char *txtFile, const char *binFile)
{
    FILE *input = fopen(txtFile, "r");
    FILE *output = fopen(binFile, "wb");
    Register reg;

    if (input == NULL || output == NULL)
    {
        perror("Erro ao tentar abrir arquivo");
        exit(1);
    }
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
void binaryToTxt(const char *binaryFileName, const char *txtFileName)
{
    FILE *binaryFile = fopen(binaryFileName, "rb");
    if (binaryFile == NULL)
    {
        perror("Erro ao abrir o arquivo binário");
        return;
    }

    FILE *txtFile = fopen(txtFileName, "w");
    if (txtFile == NULL)
    {
        perror("Erro ao criar o arquivo de texto");
        fclose(binaryFile);
        return;
    }

    Register reg;

    while (fread(&reg, sizeof(Register), 1, binaryFile) == 1)
    {
        fprintf(txtFile, "%ld ", reg.mat);
        fprintf(txtFile, "%.2f ", reg.grade);
        fprintf(txtFile, "%s ", reg.state);
        fprintf(txtFile, "%s ", reg.city);
        fprintf(txtFile, "%s\n", reg.course);
    }

    fclose(binaryFile);
    fclose(txtFile);
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
        if (reg.mat == -1)
        {
            countBlock++;
            printf("Bloco %d: \n", countBlock);
            continue;
        }
        // printf("Mat: %ld\n", reg.mat);
        printf("Grade: %.1f - Mat: %ld", reg.grade, reg.mat);
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
        buildHeap(heap, MAX_HEAP);

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

int intercalate()
{
    FILE *inputTapes[MAX_INPUT_TAPES];                 // Fitas de entrada
    FILE *outputTape = fopen("outputTape.bin", "w+b"); // Fita saida

    /* Abre as fitas de entrada */
    for (int i = 0; i < MAX_INPUT_TAPES; i++)
    {
        char nome[50];
        sprintf(nome, "input_tape%d.bin", i + 1);
        inputTapes[i] = fopen(nome, "r+b");
    }

    // Memoria interna
    ItemsHeap heap[MAX_HEAP];

    // Quantidade de fitas que terminaram de ser intercaladas
    short countTapesFinished = 0;

    // Quantidade de blocos que foram terminados de ler
    short countBlocksFinished = 0;
    short countBlocksTotal = 0;

    while (countTapesFinished < MAX_INPUT_TAPES)
    {
        // Colocar primeiros valores de cada bloco na memoria interna:
        for (int i = 0; i < MAX_INPUT_TAPES; i++)
        {
            short c = fread(&(heap[i].regs), sizeof(Register), 1, inputTapes[i]);
            // Se nao ler mais registros, quer dizer que chegou no fim daquela fita
            if (c == 0)
            {
                countBlocksFinished++;
                countTapesFinished++;
                heap[i].isMarked = true; // marca como true para que eles nao sejam levados em conta ao construir heap
            }
            else
            {
                // Marcado inicia como falso
                heap[i].isMarked = false;
                if(heap[i].regs.mat == -1)
                    heap[i].isMarked = true;
                // Numero a que fita o registro se refere
                heap[i].numTape = i;
            }
        }

        // Construir heap
        buildHeap(heap, MAX_HEAP);

        // Escreve na saida o elemento mais a esquerda do heap
        fwrite(&(heap[0].regs), sizeof(Register), 1, outputTape);
        printf("Escreveu %lf - %ld\n", heap[0].regs.grade, heap[0].regs.mat);
        //  Fita em que esta sendo percorrida atualmente
        short currentTape = heap[0].numTape;

        // Enquanto quantidade de blocos finalizados < que quant. blocos por vez de cada fita
        while (countBlocksFinished < MAX_INPUT_TAPES)
        {
            // Caso nao tenha proximo elemento da mesma fita onde saiu o ultimo menor item
            if (fread(&(heap[0].regs), sizeof(Register), 1, inputTapes[currentTape]) == 0)
            {

                heap[0].isMarked = true;
                countBlocksFinished++;
                countTapesFinished++;
            }
            // Caso tenha chegado no fim do bloco
            else if (heap[0].regs.mat == -1)
            {
                heap[0].isMarked = true;
                countBlocksFinished++;
                countBlocksTotal++;
            }
            buildHeap(heap, MAX_INPUT_TAPES);
            printf("Escreveu %lf - %ld\n", heap[0].regs.grade, heap[0].regs.mat);
            fwrite(&(heap[0].regs), sizeof(Register), 1, outputTape);
            currentTape = heap[0].numTape;
        }
        countBlocksFinished = 0;
    }

    /* Fechando fitas */
    for (int i = 0; i < MAX_INPUT_TAPES; i++)
        fclose(inputTapes[i]);

    fclose(outputTape);

    return countBlocksTotal;
}

void callIntercalate()
{
    Register reg;
    FILE *inputTapes[MAX_INPUT_TAPES]; // Fitas de entrada
    FILE *output = fopen("outputTape.bin", "r+b");
    int x = 0;
    short quantBlocksInOutput = intercalate();
    printf("X: %d\n", quantBlocksInOutput);
    x++;

    // Abre as fitas de entrada

    // Enquanto houver mais blocos que entradas, pode intercalar
    while(quantBlocksInOutput > MAX_INPUT_TAPES){
        output = fopen("outputTape.bin", "r+b");
        for (int i = 0; i < MAX_INPUT_TAPES; i++)
        {
            char nome[50];
            sprintf(nome, "input_tape%d.bin", i + 1);
            inputTapes[i] = fopen(nome, "w+b");
        }
        int index = 0;
        while (!feof(output))
        {
            fread(&(reg), sizeof(Register), 1, output);
            fwrite(&(reg), sizeof(Register), 1, inputTapes[index % MAX_INPUT_TAPES]);
            if (reg.mat == -1)
                index++;
        }

        for (int i = 0; i < MAX_INPUT_TAPES; i++)
            fclose(inputTapes[i]);
        fclose(output);

        quantBlocksInOutput = intercalate();
        printf("X: %d\n", quantBlocksInOutput);
        x++;
    }

    FILE *formatedOutput = fopen("finalOutput.bin", "wb");
    output = fopen("outputTape.bin", "rb");

    while (fread(&reg, sizeof(Register), 1, output) == 1) {
        // Verifica se o grade é diferente de -1
        if (reg.grade != -1 && reg.mat < 99999999) {
            // Se for diferente, escreve o registro no novo arquivo
            fwrite(&reg, sizeof(Register), 1, formatedOutput);
        }
    }

    // Fecha os arquivos
    fclose(output);
    fclose(formatedOutput);

    printf("Fez %d vezes\n", x);
}

int main()
{
    /* Ler e converter arquivo */
    textToBinary(NAMETXT, NAMEBIN);

    /* Gerar blocos ordenados */
    generateOrderedBlocks(NAMEBIN);
    //readBinaryFile(NAMEBIN);
    printf("----------------------\nIntercalacao:\n");
    callIntercalate();

    readBinaryFile("finalOutput.bin");
    // binaryToTxt("input_tape1.bin", "Xsaida1.txt");
    // binaryToTxt("input_tape2.bin", "Xsaida2.txt");
    // binaryToTxt("input_tape3.bin", "Xsaida3.txt");
    binaryToTxt("finalOutput.bin", "saida.txt");

    return 0;
}

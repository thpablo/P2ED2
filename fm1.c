#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "heap.h"
#include <string.h>
/* ------------------------------ METODOS SELECAO -----------------------------------*/
tItem leitor_de_registros(FILE *arquivo)
{
    tItem registro;
    char backspace;

    // lê inscricao
    fscanf(arquivo, "%8ld", &registro.inscricao);

    // lê o espaco
    fscanf(arquivo, "%c", &backspace);

    // lê a nota
    fscanf(arquivo, "%5lf", &registro.nota);

    fscanf(arquivo, "%c", &backspace);

    fscanf(arquivo, "%2s", registro.estado);

    fscanf(arquivo, "%c", &backspace);

    // fscanf(arquivo,"%50s", &registro.cidade);
    fgets(registro.cidade, 50, arquivo);

    fscanf(arquivo, "%c", &backspace);

    // fscanf(arquivo, "%30s", &registro.curso);
    fgets(registro.curso, 30, arquivo);

    // printa_Arquivo(registro);

    return registro;
}

/* Gera blocos por selecao */
void selecao_por_substituicao(int numRegs, int *comparacoes)
{
    /* Variaveis para analise */
    int transferenciasWRITE_INTERNA_PARA_EXTERNA = 0;
    int transferenciasREAD_EXTERNA_PARA_INTERNA = 0;

    Heap *heap;
    heap = criarHeap(MAX_HEAP);

    // heap construido, agora recebe os registros e joga pro heap
    FILE *data = fopen("bin/data.bin", "rb");
    char nomeFitaEntrada[23] = "bin/fita00.bin";
    FILE *arquivoFita = fopen(nomeFitaEntrada, "ab");
    tItem marcaFim = {-1, 0, "", "", ""};

    if (data == NULL)
        printf("\nFalha ao abrir o arquivo\n");
    if (arquivoFita == NULL)
        printf("\nFalha ao abrir o arquivo\n");

    int total = 0;
    int contadorRegistros = 0;
    // recebe os itens do provao, adiciona no registro com o marcador 0 e insere no heap
    int quantLerMemoriaInterna = (numRegs <= MAX_HEAP) ? numRegs : MAX_HEAP;
    for (int i = 0; i < quantLerMemoriaInterna; i++)
    {
        tItem itemAux;
        tRegistro regAux;

        fread(&(itemAux), sizeof(tItem), 1, data);
        transferenciasREAD_EXTERNA_PARA_INTERNA++;

        regAux.item = itemAux;
        regAux.marcador = 0;
        inserir(heap, regAux, comparacoes); //Adiciona no heap
    }

    int contaItensMarcados = 0;
    int numeroDaFita = 0;
    tRegistro novoReg, minReg;
    int jaEscreveuMarcaFim = 0;

    do
    {
        do
        {
            // comparando o menor do heap com o novo registro para saber se o novo sera marcado
            minReg = extrairMinimo(heap, comparacoes);

            total++;
            contadorRegistros++;

            fwrite(&minReg.item, sizeof(tItem), 1, arquivoFita);
            transferenciasWRITE_INTERNA_PARA_EXTERNA++;

            if (contadorRegistros >= numRegs)
            {
                fwrite(&marcaFim, sizeof(tItem), 1, arquivoFita);
                transferenciasWRITE_INTERNA_PARA_EXTERNA++;

                jaEscreveuMarcaFim = 1;
                break;
            }
            fread(&(novoReg.item), sizeof(tItem), 1, data);
            transferenciasREAD_EXTERNA_PARA_INTERNA++;

            (*comparacoes)++;
            if (minReg.item.nota > novoReg.item.nota)
            {
                novoReg.marcador = 1;
                contaItensMarcados++;
            }
            else
                novoReg.marcador = 0;

            inserir(heap, novoReg, comparacoes);
        } while (contaItensMarcados < MAX_HEAP || minReg.marcador == 1);
        contaItensMarcados = 0;

        // Desmarco o Heap
        desmarcaHeap(heap);

        // printa um marcador de fim de bloco, onde a inscricao eh -1
        if (!jaEscreveuMarcaFim) //Evitar escreverem multiplos -1
        {
            fwrite(&marcaFim, sizeof(tItem), 1, arquivoFita);
            transferenciasWRITE_INTERNA_PARA_EXTERNA++;
        }

        jaEscreveuMarcaFim = 0;

        // fecha a fita e abre a proxima
        fclose(arquivoFita);

        /* Extrai o numero da fita atual e incrementa para mudar de fita de entrada*/
        if (sscanf(nomeFitaEntrada, "bin/fita%02d.bin", &numeroDaFita) != 1)
        {
            fprintf(stderr, "Erro ao extrair o número da fita\n");
            return;
        }
        numeroDaFita = (numeroDaFita + 1) % MAX_INPUT_TAPES;
        sprintf(nomeFitaEntrada, "bin/fita%02d.bin", numeroDaFita);

        /*Abre fita de entrada seguinte */
        fopen(nomeFitaEntrada, "ab");
    } while (contadorRegistros < numRegs);
    fclose(data);
    desalocaHeap(heap);

    printf("---- METODO DE SELECAO ----\n");
    printf("NUMERO DE TRANSFERENCIAS DE ESCRITAS:\t\t %d\n", transferenciasWRITE_INTERNA_PARA_EXTERNA);
    printf("NUMERO DE TRANSFERENCIAS DE LEITURAS:\t\t %d\n", transferenciasREAD_EXTERNA_PARA_INTERNA);
}

/* Le arquivo provao */
void readProvao(const char *txtFile, const char *binFile, long numReg)
{
    /* Abre as fitas */
    FILE *input = fopen(txtFile, "r");
    FILE *output = fopen(binFile, "wb");
    tItem reg;
    long cont = 0;

    if (input == NULL || output == NULL)
    {
        perror("Erro ao tentar abrir arquivo");
        exit(1);
    }

    /* Le provao conforme registros*/
    while (cont < numReg)
    {
        fscanf(input, "%ld %lf", &reg.inscricao, &reg.nota);
        fgets(reg.estado, MAX_STATE, input);
        fgets(reg.cidade, MAX_CITY, input);
        fgets(reg.curso, MAX_COURSE, input);
        fwrite(&reg, sizeof(tItem), 1, output);
        cont++;
    }

    fclose(input);
    fclose(output);
}

/* Cria fitas de entrada */
void createTapes(int n)
{
    // Nome para as fitas de entradas (Maximo 100 fitas)
    char nomeFitaEntrada[23];
    FILE *arquivoFita;

    /* Cria N fitas de entrada */
    for (int i = 0; i < n; i++)
    {
        sprintf(nomeFitaEntrada, "bin/fita%02d.bin", i);
        arquivoFita = fopen(nomeFitaEntrada, "w+b");
        if (arquivoFita == NULL)
        {
            perror("Erro ao criar o arquivo de fita");
            return;
        }
        fclose(arquivoFita);
    }
}

/* Inicia fitas para selecao */
void initTapes(int numRegs, int *comparacoes)
{
    createTapes(MAX_INPUT_TAPES);                   // Cria fitas resetadas para desconsiderar possiveis antigas
    selecao_por_substituicao(numRegs, comparacoes); // Gera blocos Ordenados
}

/* ----------- FIM Funcoes de Selecao por Substituicao -------------------------------*/

/* Converte arquivo binario para txt */
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

    /* Leitura dos registros */
    while (fread(&reg, sizeof(Register), 1, binaryFile) == 1)
    {
        fprintf(txtFile, "%08ld ", reg.mat);     // Matrícula com zeros à esquerda e ocupando 6 posições
        fprintf(txtFile, "%05.1f ", reg.grade);  // Nota com 3 zeros à esquerda e 1 casa decimal
        fprintf(txtFile, "%-2s", reg.state);    // Estado com 2 caracteres à direita
        fprintf(txtFile, "%-50s", reg.city);    // Cidade com 50 caracteres à esquerda
        fprintf(txtFile, "%-30s \n", reg.course); // Curso com 30 caracteres à esquerda
    }

    fclose(binaryFile);
    fclose(txtFile);
}

/* ---- Funcoes Heap que funciona so pra Intercalacao -------- */

// Funcao troca dois itens
void swap(ItemsHeap *a, ItemsHeap *b)
{
    ItemsHeap temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(ItemsHeap heap[], int n, int i, int* comparacoes)
{
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    (*comparacoes)++;
    // Ordenar primeiro por marcado e depois por nota em caso de empate
    if (left < n &&
        (heap[left].isMarked < heap[smallest].isMarked ||
         (heap[left].isMarked == heap[smallest].isMarked &&
          heap[left].regs.grade < heap[smallest].regs.grade))) {
        smallest = left;
    }

    (*comparacoes)++;
    if (right < n &&
        (heap[right].isMarked < heap[smallest].isMarked ||
         (heap[right].isMarked == heap[smallest].isMarked &&
          heap[right].regs.grade < heap[smallest].regs.grade))) {
        smallest = right;
    }

    if (smallest != i)
    {
        swap(&heap[i], &heap[smallest]);
        heapify(heap, n, smallest, comparacoes);
    }
}

void buildHeap(ItemsHeap heap[], int n, int *comparacoes)
{
    int startIdx = (n / 2) - 1;

    for (int i = startIdx; i >= 0; i--)
    {
        heapify(heap, n, i, comparacoes);
    }
}

/* Intercalacao Balanceada F + 1 */
int intercalateFM1(int *transfRead, int *transfWrite, int *comparacoes)
{
    FILE *inputTapes[MAX_INPUT_TAPES];                     // Fitas de entrada
    FILE *outputTape = fopen("bin/outputTape.bin", "w+b"); // Fita saida

    /*ABRE FITAS*/
    char nomeFitaEntrada[15];

    // Abre as fitas de entrada
    for (int i = 0; i < MAX_INPUT_TAPES; i++)
    {
        sprintf(nomeFitaEntrada, "bin/fita%02d.bin", i);
        inputTapes[i] = fopen(nomeFitaEntrada, "r+b");
    }

    // Memoria interna
    ItemsHeap heap[MAX_HEAP];

    // Quantidade de fitas que terminaram de ser intercaladas
    int countTapesFinished = 0;

    // Quantidade de blocos que foram terminados de ler
    int countBlocksFinished = 0;

    // Aumenta quantidade de blocos totais gerados na intercalacao
    int countBlocksTotal = 0;

    while (countTapesFinished < MAX_INPUT_TAPES)
    {
        // Colocar primeiros valores de cada bloco na memoria interna:
        for (int i = 0; i < MAX_INPUT_TAPES; i++)
        {
            // Le primeiro registro dos blocos
            short c = fread(&(heap[i].regs), sizeof(Register), 1, inputTapes[i]);
            *transfRead = (*transfRead) + 1;

            // Se nao ler mais registros ou ler registro que indicafim do bloco,
            // significa que chegou no fim daquela fita
            if (c == 0 || heap[i].regs.mat == -1)
            {
                // Aumenta quantidade de Blocos e fitas finalizados
                countBlocksFinished++;
                countTapesFinished++;
                heap[i].isMarked = true; // marca como true para que eles nao sejam levados em conta ao construir heap
            }
            else
            {
                // Marcado inicia como falso
                heap[i].isMarked = false;
                // Numero a que fita o registro se refere
                heap[i].numTape = i;
            }
        }

        // Construir heap para os primeiros valores
        buildHeap(heap, MAX_HEAP, comparacoes);

        // Escreve na saida o menor elemento na posicao 0 do heap
        fwrite(&(heap[0].regs), sizeof(Register), 1, outputTape);
        *transfWrite = (*transfWrite) + 1;

        //   Fita em que esta sendo percorrida atualmente
        short currentTape = heap[0].numTape;

        // Enquanto quantidade de blocos finalizados < que quant. de fitas
        while (countBlocksFinished < MAX_INPUT_TAPES)
        {
            // Caso nao tenha proximo elemento da mesma fita onde saiu o ultimo menor item
            if (fread(&(heap[0].regs), sizeof(Register), 1, inputTapes[currentTape]) == 0)
            {
                // Marca essa posicao para ter peso maior (*)
                heap[0].isMarked = true;
                // Indica que finalizou um bloco
                countBlocksFinished++;
                // Indica que finalizou uma fita
                countTapesFinished++;
            }
            // Caso tenha chegado no fim do bloco
            else if (heap[0].regs.mat == -1)
            {
                // Marca essa posicao para ter peso maior (*)
                heap[0].isMarked = true;
                // Aumenta quantidade de blocos finalizados
                countBlocksFinished++;
                // Aumenta quantidade de blocos totais
                countBlocksTotal++;
            }

            *transfRead = (*transfRead) + 1;
            // Constroi heap com os valores intercalados
            buildHeap(heap, MAX_HEAP, comparacoes);

            // Escreve na saida o menor elemento (posicao 0 do heap)
            fwrite(&(heap[0].regs), sizeof(Register), 1, outputTape);
            *transfWrite = (*transfWrite) + 1;

            // Indica qual fita esta sendo intercalada (sempre a do menor registro - posicao 0)
            currentTape = heap[0].numTape;
        }
        // Zera quantidade de blocos finalizados para intercalar outros blocos
        countBlocksFinished = 0;
    }

    /* Fechando fitas */
    for (int i = 0; i < MAX_INPUT_TAPES; i++)
        fclose(inputTapes[i]);

    fclose(outputTape);

    //  Retorna a quantidade de blocos gerados na saida
    return countBlocksTotal;
}

/* Escreve os registros validos na saida final */
void formatFinalOutput(FILE *output, FILE *formatedOutput)
{
    Register reg;

    // Ler os registros de output e colocar em formatedOutput
    while (fread(&reg, sizeof(Register), 1, output) == 1)
    {
        // Escrever somente se a matricula for diferente de 0, 1 e se reg.state ou reg.city ou reg.couse estiverem vazios
        if (reg.mat != 0 && reg.mat != 1 && !(reg.state[0] == '\0' || reg.city[0] == '\0' || reg.course[0] == '\0'))
        {
            fwrite(&reg, sizeof(Register), 1, formatedOutput);
        }
    }
}

/* Chama a funcao de intercalar a quantidade de vezes necessaria */
void callIntercalate(int *comparacoes)
{
    Register reg;
    FILE *inputTapes[MAX_INPUT_TAPES]; // Fitas de entrada
    FILE *output;                      // Fita de Saida
    char nomeFitaEntrada[15];

    /* Variaveis para analise */
    int transferenciasWRITE_INTERNA_PARA_EXTERNA = 0;
    int transferenciasREAD_EXTERNA_PARA_INTERNA = 0;

    // Numero de vezes intercaladas
    int numberTimesIntercalate = 0;

    /* Intercala a primeira vez */
    int quantBlocksInOutput = intercalateFM1(&transferenciasREAD_EXTERNA_PARA_INTERNA, &transferenciasWRITE_INTERNA_PARA_EXTERNA, comparacoes);
    // quantidade de blocos na saida
    numberTimesIntercalate++;

    /* Intercala outras vezes */
    // Enquanto houver mais blocos que entradas, pode intercalar
    while (quantBlocksInOutput > MAX_INPUT_TAPES)
    {
        /* OLhando se ele separa certo os blocos para as fitas de entrada */
        int index = 0;
        output = fopen("bin/outputTape.bin", "r+b");
        for (int i = 0; i < MAX_INPUT_TAPES; i++)
        {
            sprintf(nomeFitaEntrada, "bin/fita%02d.bin", i);
            inputTapes[i] = fopen(nomeFitaEntrada, "w+b");
        }

        /* fazer ele ler o output e colocar nas fitas de entrada. Depois ver se ta colocando o -1 certo */
        Register regAnterior;
        while (!feof(output))
        {
            fread(&(reg), sizeof(Register), 1, output);
            transferenciasREAD_EXTERNA_PARA_INTERNA = transferenciasREAD_EXTERNA_PARA_INTERNA + 1;
            if (!(regAnterior.mat == -1 && reg.mat == -1))
            {
                fwrite(&(reg), sizeof(Register), 1, inputTapes[index % MAX_INPUT_TAPES]);
                transferenciasWRITE_INTERNA_PARA_EXTERNA = transferenciasWRITE_INTERNA_PARA_EXTERNA + 1;
            }
            if (reg.mat == -1)
            {
                index++;
            }
            regAnterior = reg;
        }

        for (int i = 0; i < MAX_INPUT_TAPES; i++)
            fclose(inputTapes[i]);

        fclose(output);
        quantBlocksInOutput = intercalateFM1(&transferenciasREAD_EXTERNA_PARA_INTERNA, &transferenciasWRITE_INTERNA_PARA_EXTERNA, comparacoes);
        numberTimesIntercalate++;
    }
    printf("---------- ** ----------\n");
    printf(" Fez %d intercalacoes\n", numberTimesIntercalate);
    printf("---------- ** ----------\n");

    printf("-------- Transferencias Intercalacao --------\n");
    printf("Transferencias numero Escritas:\t\t %d\n", transferenciasWRITE_INTERNA_PARA_EXTERNA);
    printf("Transferencias numero de Leituras:\t %d\n", transferenciasWRITE_INTERNA_PARA_EXTERNA);
}

/* FOrmata saida final retirando valores com registros Jump */
void callFormatFinalOutput()
{
    FILE *output = fopen("bin/outputTape.bin", "r+b");
    FILE *formatedOutput = fopen("bin/finalOutput.bin", "w+b");

    formatFinalOutput(output, formatedOutput);
    fclose(output);
    fclose(formatedOutput);

    binaryToTxt("bin/finalOutput.bin", "SaidaFinal.txt");
}

/* Escolhe qual arquivo base sera usado */
void switchNameFile(char *nameFile, int situacao)
{
    switch (situacao)
    {
    case 1:
        strcpy(nameFile, "ascendente.txt");
        break;
    case 2:
        strcpy(nameFile, "descendente.txt");
        break;
    case 3:
        strcpy(nameFile, "PROVAO.TXT");
        break;
    default:
        printf("Insira uma situacao valida");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{

    printf("nota: Colocar funcao -P...\n");
    int numRegs = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int comparacoes = 0;

    char nameFile[30];
    switchNameFile(nameFile, situacao);

    // Le provao e converte para binario
    readProvao(nameFile, NAMEBIN, numRegs);

    // Gera blocos ordenados
    initTapes(numRegs, &comparacoes);

    int comparacoesBlocosOrdenados = comparacoes;
    printf("\nNumero de Comparacoes para gerar blocos ordenados: %d\n\n", comparacoesBlocosOrdenados);


    /* Chama intercalacao */
    callIntercalate(&comparacoes);

    printf("\nNumero de Comparacoes para intercalacao: %d\n", (comparacoes - comparacoesBlocosOrdenados));

    /* Formata saida Final*/
    callFormatFinalOutput();
    printf("---------- ** ----------\n");
    printf("Comparacoes totais: %d\n", comparacoes);
    return 0;
}

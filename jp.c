#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "structures.h"

void createTapes(int n){
    char nomeFitaEntrada[23]; // Assuming the maximum value of n is 99
    FILE *arquivoFita;

    for (int i = 0; i < n; i++) {
        sprintf(nomeFitaEntrada, "bin/fita%02d.bin", i);
        arquivoFita = fopen(nomeFitaEntrada, "w+b");
        if (arquivoFita == NULL) {
            perror("Erro ao criar o arquivo de fita");
            return;
        }
        fclose(arquivoFita);
    }
}

int somatorioVetor(int vetor[],int n){
    int somatorio = 0;
    for (int i = 0; i < n; i++)
    {
        somatorio = somatorio + vetor[i];
    }
    return somatorio;
}


void intercalacao_Balanceada_Fitas_entrada(){

    // definicao dos nomes das fitas de entrada e saida
    char nomeArquivoEntrada[] = "bin/fita00.bin";
    char nomeArquivoSaida[] = "bin/saida.bin";
    

    //define_Nome_Fita_Saida(nomeArquivoSaida);

    //vetor de ponteiros de arquivos para a leitura e escrita das fitas
    FILE *arquivosEntrada[MAX_INPUT_TAPES];
    FILE *arquivosSaida;
    arquivosSaida = fopen(nomeArquivoSaida,"wb");
    

    //Abre os arquivos de fita de entrada
    for (int i = 0; i < (MAX_INPUT_TAPES); i++)
    {
        arquivosEntrada[i] = fopen(nomeArquivoEntrada,"rb");
        sprintf(nomeArquivoEntrada, "bin/fita%02d.bin", i);

    }

    //variavel auxiliar
    tItem auxItem;
    //simulação da memeoria interna
    tItem registros[MAX_HEAP];
    tItem marcaFim = {-1,0,"","",""};
    int c;

    int atvFitas[MAX_INPUT_TAPES],marcador=0;
    int k = 0;
    int acabaLoop=0;
    do{    
        //lendo os primeiros f registros, adiciono numa estrutura de dados ja ordenada
        //atualizo a atividade de cada fita 
        for (int i = 0; i < MAX_INPUT_TAPES; i++)
        {
            atvFitas[i] = 1;
            c = fread(&registros[i],sizeof(tItem),1,arquivosEntrada[i]);
            if(c != 1){
                atvFitas[i] = 0;
                }

            if(registros[i].inscricao < 0)
                atvFitas[i] = 0;
        }

        if(somatorioVetor(atvFitas,(MAX_INPUT_TAPES)) == 0) break;
        //processo de intercalação dos blocos
        
        do{

            if(somatorioVetor(atvFitas,(MAX_INPUT_TAPES)) == 0) break;
            //Defino o elemento base para comparar
            for (int i = 0; i < (MAX_INPUT_TAPES); i++)
            {
                if(atvFitas[i] == 1) {
                    auxItem = registros[i];
                    break;
            }
            }
            
            for (int i = 0; i < (MAX_INPUT_TAPES); i++){
                if(atvFitas[i] == 0){
                    continue;
                }else if(registros[i].nota <= auxItem.nota){
                    auxItem = registros[i];
                    marcador = i;
                }
            }
            

                fwrite(&auxItem,sizeof(tItem),1,arquivosSaida);
            if(fread(&registros[marcador],sizeof(tItem),1,arquivosEntrada[marcador]) != 1){
                acabaLoop =1;
                break;
                atvFitas[marcador] = 0;
            }
            if(registros[marcador].inscricao <= 0) {
                atvFitas[marcador] = 0;
            }
            

            
        }while(somatorioVetor(atvFitas,(MAX_INPUT_TAPES)) != 0);
        //finaliza o bloco
        
        if(acabaLoop == 1) break;
        fwrite(&marcaFim,sizeof(tItem),1,arquivosSaida);
        if(k < (MAX_INPUT_TAPES)-1){
            k++;
        }else k=0;

       
        
    }while(1);    
    
    
    //fecha os arquivos abertos
    for (int i = 0; i < (MAX_INPUT_TAPES); i++)
    {
        fclose(arquivosEntrada[i]);
    }
    fclose(arquivosSaida);

}

/* Troca de fita de saida para fita de entrada */


/*

void intercalacao_Balanceada_Fitas_Saida(){
    
    // definicao dos nomes das fitas de entrada e saida
    char nomeArquivoEntrada[] = "bin/fita00.bin";
    
    char nomeArquivoSaida[] = "bin/saida.bin";
    define_Nome_Fita_Saida(nomeArquivoSaida);

    //vetor de ponteiros de arquivos para a leitura e escrita das fitas
    FILE *arquivosEntrada[MAX_INPUT_TAPES];
    FILE *arquivosSaida;
    arquivosSaida = fopen(nomeArquivoSaida,"rb");

    //Abre os arquivos de fita de entrada
    for (int i = 0; i < (MAX_INPUT_TAPES); i++)
    {
        arquivosEntrada[i] = fopen(nomeArquivoEntrada,"wb");
        sprintf(nomeArquivoEntrada, "bin/fita%02d.bin", i);
    }

    //variavel auxiliar
    tItem auxItem;
    //simulação da memeoria interna
    tItem registros[MAX_HEAP];
    tItem marcaFim = {-1,0,"","",""};

    int atvFitas[MAX_INPUT_TAPES],marcador=0;
    int k = 0;
    int acabaLoop=0;
    do{    
        //lendo os primeiros f registros, adiciono numa estrutura de dados ja ordenada
        //atualizo a atividade de cada fita 
        for (int i = 0; i < MAX_INPUT_TAPES; i++)
        {
            atvFitas[i] = 1;

            if(fread(&registros[i],sizeof(tItem),1,arquivosSaida[i]) != 1)
                atvFitas[i] = 0;

            if(registros[i].inscricao < 0)
                atvFitas[i] = 0;
        }

        if(somatorioVetor(atvFitas,(MAX_INPUT_TAPES)) == 0) break;
        //processo de intercalação dos blocos
        
        do{

            if(somatorioVetor(atvFitas,(MAX_INPUT_TAPES)) == 0) break;
            //Defino o elemento base para comparar
            for (int i = 0; i < (MAX_INPUT_TAPES); i++)
            {
                if(atvFitas[i] == 1) {
                    auxItem = registros[i];
                    break;
            }
            }
            
            for (int i = 0; i < (MAX_INPUT_TAPES); i++){
                if(atvFitas[i] == 0){
                    continue;
                }else if(registros[i].nota <= auxItem.nota){
                    auxItem = registros[i];
                    marcador = i;
                }
            }
            fwrite(&auxItem,sizeof(tItem),1,arquivosEntrada[k]);
            if(fread(&registros[marcador],sizeof(tItem),1,arquivosSaida[marcador]) != 1){
                acabaLoop =1;
                break;
                atvFitas[marcador] = 0;
            }
            if(registros[marcador].inscricao <= 0) {
                atvFitas[marcador] = 0;
            }
            

            
        }while(somatorioVetor(atvFitas,(NUM_FITAS/2)) != 0);
        //finaliza o bloco
        
        if(acabaLoop == 1) break;
        fwrite(&marcaFim,sizeof(tItem),1,arquivosEntrada[k]);
        if(k < (NUM_FITAS/2)-1){
            k++;
        }else k=0;

       
        
    }while(1);    
    
    
    //fecha os arquivos abertos
    for (int i = 0; i < (NUM_FITAS/2); i++)
    {
        fclose(arquivosEntrada[i]);
        fclose(arquivosSaida[i]);
    }



}
*/

void sobreescreverArquivos() {
    FILE *arquivoSaida = fopen("bin/saida.bin", "rb");
    if (arquivoSaida == NULL) {
        printf("\nFalha na abertura do arquivo de saída...");
        return;
    }

    FILE *arquivosFita[MAX_INPUT_TAPES];
    char nomeArquivo[23];

    // Abrir os arquivos fitaXX para escrita
    for (int i = 0; i < MAX_INPUT_TAPES; i++) {
        sprintf(nomeArquivo, "bin/fita%02d.bin", i);
        arquivosFita[i] = fopen(nomeArquivo, "wb");
        if (arquivosFita[i] == NULL) {
            printf("\nFalha na abertura do arquivo %s...", nomeArquivo);
            return;
        }
    }

    tItem registro;
    int fitaAtual = 0;

    while (fread(&registro, sizeof(tItem), 1, arquivoSaida) == 1) {
        fwrite(&registro, sizeof(tItem), 1, arquivosFita[fitaAtual % MAX_INPUT_TAPES]);
        if (registro.inscricao == -1) {
            fitaAtual++;
            continue;
        }
    }

    fclose(arquivoSaida);

    // Fechar os arquivos fitaXX
    for (int i = 0; i < MAX_INPUT_TAPES; i++) {
        fclose(arquivosFita[i]);
    }
}



int confere_Intercalacao(){
    FILE *arquivo = fopen("bin/saida.bin","rb");
    if(arquivo == NULL){
        printf("\nFalha na abertura do arquivo...");
        return -1;
    }

    int quantidade = 0;
    tItem aux;
    aux.inscricao = -2;
     
   do{
        if(fread(&aux,sizeof(tItem),1,arquivo) != 1)break;
        if(aux.inscricao < 0){
            break;
        }
        quantidade++;
    }while(1);
    
    
    fclose(arquivo);
    return (quantidade);
}

int calculaQuantidadeBlocos(FILE* arquivo){
    int quantidade = 0;
    tItem itemAux;
    while(fread(&itemAux,sizeof(tItem),1,arquivo) != 0){
        if(itemAux.nota == -1) break;
        quantidade++;
    }
    return quantidade;
    
}

void imprime_conteudo_fita(){
    char nomeArquivo[] = "bin/fita00.bin";
    int c=0;
    FILE* arquivo;
    for(int i = 0;i< MAX_INPUT_TAPES;i++){
        
        arquivo = fopen(nomeArquivo,"rb");
        printf("\n\nFita: %s",nomeArquivo);
        tItem aux;
        while(fread(&aux,sizeof(tItem),1,arquivo) != 0){
            if(aux.nota == -1) {
                printf("\n -----------------");
                break;
            }else{ 
                printf("\n%d)%ld - %lf",c,aux.inscricao,aux.nota);
                c++;
            }
        }
        fclose(arquivo);
    }
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

    tItem reg;

    while (fread(&reg, sizeof(tItem), 1, binaryFile) == 1)
    {
        fprintf(txtFile, "%ld ", reg.inscricao);
        fprintf(txtFile, "%.2f ", reg.nota);
        fprintf(txtFile, "%s ", reg.estado);
        fprintf(txtFile, "%s ", reg.cidade);
        fprintf(txtFile, "%s\n", reg.curso);
    }

    fclose(binaryFile);
    fclose(txtFile);
}


int main(){
    printf("\nCriacao das fitas para intercalacao...");

    printf("\n\n... Fase de selecao por substituicao ... ");
    //selecao_por_substituicao();
    printf("\n\n... Fase de intercalacao ... ");


    int i = 0;
    char nomeFitaEntrada[23];
    do{
        intercalacao_Balanceada_Fitas_entrada();
        sobreescreverArquivos();
        sprintf(nomeFitaEntrada, "bin/fita%d00.txt", i); binaryToTxt("bin/fita00.bin", nomeFitaEntrada);
        sprintf(nomeFitaEntrada, "bin/fita%d01.txt", i); binaryToTxt("bin/fita01.bin", nomeFitaEntrada);
        sprintf(nomeFitaEntrada, "bin/fita%d02.txt", i); binaryToTxt("bin/fita02.bin", nomeFitaEntrada);

        i++; printf("%d\n ", i);
    }while(i < 2);

    printf("\n\n\n...Gerando registros ordenados...");
    binaryToTxt("bin/saida.bin", "bin/final.txt");
    
    //gera_registros_ordenados();

    return 0;
}
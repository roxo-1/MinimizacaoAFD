
// Remover estados inacessíveis (se houver). 
// Minimizar o autômato. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definindo limites
#define MAX_LINHAS 100
#define TAM_LINHAS 256

#define MAX_ALFABETO 50
#define TAM_ALFABETO 50 

#define MAX_ESTADOS 50
#define TAM_ESTADOS 50 

#define MAX_TRANSICOES 100
#define TAM_TRANSICOES 100

#define MAX_PALAVRAS 100
#define TAM_PALAVRAS 100

typedef struct{
    char texto[MAX_LINHAS][TAM_LINHAS]; //matriz
    int qtd; //qtd de linhas
}ListaDeLinhas;

typedef struct{
    char alfabeto[MAX_ALFABETO][TAM_ALFABETO];
    int qtd_alfabeto;

    char estados[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_estados;

    char estado_inicial[TAM_ESTADOS]; // Apenas 1 estado inicial

    char estados_finais[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_finais;

    char transicoes[MAX_TRANSICOES][TAM_TRANSICOES];
    int qtd_transicoes;

    char palavras[MAX_PALAVRAS][TAM_PALAVRAS];
    int qtd_palavras;
}AFD; //INICIAL

typedef struct MAFD
{
    char alfabeto[MAX_ALFABETO][TAM_ALFABETO];
    int qtd_alfabeto;

    char estados[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_estados;

    char estado_inicial[TAM_ESTADOS]; // Apenas 1 estado inicial

    char estados_finais[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_finais;

    char transicoes[MAX_TRANSICOES][TAM_TRANSICOES];
    int qtd_transicoes;

    char palavras[MAX_PALAVRAS][TAM_PALAVRAS];
    int qtd_palavras;
}MAFD; //DEPOIS DAS TRANSIÇÕES, RESULTADO

// Ler e interpretar corretamente o autômato. 
void carregarArquivo(const char *Nomearquivo, ListaDeLinhas *lista){
    FILE *arquivo = fopen(Nomearquivo, "r");
    lista->qtd =0;
    if(arquivo==NULL){
        printf("Erro: Não foi possível abrir o arquivo!");
        return;
    }
    char linha[TAM_LINHAS];
    while(fgets(linha, TAM_LINHAS, arquivo) != NULL){
        if(linha[0]=='#'){
            continue; //força o loop a continuar, ignorando essa linha
        }
        if(lista->qtd >= MAX_LINHAS){ // Se a lista encher, para de ler
            printf("Limite de linhas atingido!");
            break;
        }
        // Copia a linha para a lista de listas, onde armazenaremos as linhas para usá-las depois
        strcpy(lista->texto[lista->qtd], linha);//destino -> origem, ou seja, veio do buffer "linha" e vai para a lista de lista
        lista->qtd++;
    }
    fclose(arquivo);
}

void processararAFD(ListaDeLinhas *lista, AFD *afd){
    // Inicializa os contadores
    afd->qtd_alfabeto = 0;
    afd->qtd_estados = 0;
    afd->qtd_finais = 0;
    afd->qtd_transicoes = 0;
    afd->qtd_palavras = 0;
    char buffer[TAM_LINHAS];
    
    for(int i=0; i<lista->qtd; i++){
        strcpy(buffer, lista->texto[i]);
        
        // Delimitadores: espaço, quebra de linha e tabulação
        char *token = strtok(buffer, " \r\n"); 
        
        if(token == NULL) continue;

        if(strcmp(token, "A") == 0) { // [cite: 22]
            token = strtok(NULL, " \r\n"); 
            while(token != NULL) {
                strcpy(afd->alfabeto[afd->qtd_alfabeto], token);
                afd->qtd_alfabeto++;
                token = strtok(NULL, " \r\n"); // Correção: continua buscando por espaço
            }
        } 
        else if(strcmp(token, "Q") == 0) { // [cite: 23]
            token = strtok(NULL, " \r\n"); 
            while(token != NULL) {
                strcpy(afd->estados[afd->qtd_estados], token);
                afd->qtd_estados++;
                token = strtok(NULL, " \r\n");
            }
        }
        else if(strcmp(token, "q") == 0) { // [cite: 24]
            token = strtok(NULL, " \r\n");
            if(token != NULL) {
                strcpy(afd->estado_inicial, token);
            }
        }
        else if(strcmp(token, "F") == 0) { // [cite: 25]
            token = strtok(NULL, " \r\n"); 
            while(token != NULL) {
                strcpy(afd->estados_finais[afd->qtd_finais], token);
                afd->qtd_finais++;
                token = strtok(NULL, " \r\n");
            }
        }
        else if(strcmp(token, "T") == 0) { // 
            // Para transições, vamos guardar os tokens seguintes como uma string única ou processar
            // Vamos assumir formato: T Origem Simbolo Destino
            char t_origem[50], t_simb[50], t_dest[50];
            
            char *p1 = strtok(NULL, " \r\n");
            char *p2 = strtok(NULL, " \r\n");
            char *p3 = strtok(NULL, " \r\n");

            if (p1 && p2 && p3) {
                // Formata padronizado na struct: "Origem Simbolo Destino"
                sprintf(afd->transicoes[afd->qtd_transicoes], "%s %s %s", p1, p2, p3);
                afd->qtd_transicoes++;
            }
        }
        else if(strcmp(token, "P") == 0) { // [cite: 27]
            token = strtok(NULL, " \r\n");
            while(token != NULL) {
                strcpy(afd->palavras[afd->qtd_palavras], token);
                afd->qtd_palavras++;
                token = strtok(NULL, " \r\n");
            }
        }
    }
}

// Gerar um novo arquivo contendo o AFD minimizado
void saida(MAFD *mafd, const char *fileSaida){
    FILE *file = fopen(fileSaida, "w");
    if (file == NULL) {
        printf("Erro: Nao foi possivel criar o arquivo de saida.\n");
        return; 
    }
    fprintf(file, "# Automato Finito Determinístico Minimizado\n");
    fprintf(file, "\n# Alfabeto\n");
    fprintf(file, "\nA <%s>\n", mafd->alfabeto);
    fprintf(file, "\n# Estados\n");
    fprintf(file, "\nE <%s>\n", mafd->estados);
    fprintf(file, "\n# Estado Inicial\n");
    fprintf(file, "\nq <%s>\n", mafd->estado_inicial);
    fprintf(file, "\n# Estados Finais\n");
    fprintf(file, "\nF <%s>\n", mafd->estados_finais);
    fprintf(file, "\n# Transições\n");
    fprintf(file, "\nT <%s>\n", mafd->transicoes);
}

int main(){
    ListaDeLinhas entrada;
    AFD afd;
    MAFD mfad;
    const char *file = "saida.txt";
    carregarArquivo("12.txt", &entrada);
    return 0;
}
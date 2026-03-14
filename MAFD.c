// Ler e interpretar corretamente o autômato. 
// Remover estados inacessíveis (se houver). 
// Minimizar o autômato. 
// Gerar um novo arquivo contendo o AFD minimizado
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
}AFD;

typedef struct MAFD
{
    char estado[TAM_ESTADOS];
    char palavras[MAX_PALAVRAS][TAM_PALAVRAS];
}MAFD;




int main(){
    return 0;
}
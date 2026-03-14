#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definindo limites
#define MAX_LINHAS 100
#define TAM_LINHAS 256

#define MAX_ALFABETO 26
#define TAM_ALFABETO 26 

#define MAX_ESTADOS 10
#define TAM_ESTADOS 10 

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

    bool alcancaveis[MAX_ESTADOS];
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

int buscarIndice(AFD *afd, char *nomeEstado) {
    for (int i = 0; i < afd->qtd_estados; i++) {
        if (strcmp(afd->estados[i], nomeEstado) == 0) return i;
    }
    return -1;
}

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
// Remover estados inacessíveis (se houver). 
void removerEstadosInacessiveis(AFD *afd) {
    // Inicializa alcancaveis como falso
    for (int i = 0; i < afd->qtd_estados; i++) {
        afd->alcancaveis[i] = false;
    }

    int idx_inicial = buscarIndice(afd, afd->estado_inicial);
    if (idx_inicial == -1) return;

    int fila[MAX_ESTADOS];
    int frente = 0, tras = 0;

    afd->alcancaveis[idx_inicial] = true;
    fila[tras++] = idx_inicial;

    while (frente < tras) {
        int u_idx = fila[frente++];
        char *nome_u = afd->estados[u_idx];

        // Varre as transições para ver quem o estado 'u' alcança
        for (int i = 0; i < afd->qtd_transicoes; i++) {
            char temp[TAM_TRANSICOES];
            strcpy(temp, afd->transicoes[i]); // Copia para não estragar a original com strtok
            
            char *origem = strtok(temp, " ");
            char *simbolo = strtok(NULL, " ");
            char *destino = strtok(NULL, " ");

            // Se a transição parte do estado que estamos explorando
            if (origem != NULL && strcmp(origem, nome_u) == 0) {
                int v_idx = buscarIndice(afd, destino);
                if (v_idx != -1 && !afd->alcancaveis[v_idx]) {
                    afd->alcancaveis[v_idx] = true;
                    fila[tras++] = v_idx;
                }
            }
        }
    }
}
// Minimizar o autômato. 
void inicializarParticao(AFD *afd, int grupo[]) {
    for (int i = 0; i < afd->qtd_estados; i++) {
        grupo[i] = 0; // Assume que é não-final
        for (int j = 0; j < afd->qtd_finais; j++) {
            if (strcmp(afd->estados[i], afd->estados_finais[j]) == 0) {
                grupo[i] = 1; // É final
                break;
            }
        }
    }
}

int destinoTransicao(AFD *afd, int estado_idx, char *simbolo) {
    for (int i = 0; i < afd->qtd_transicoes; i++) {
        char temp[TAM_TRANSICOES];
        strcpy(temp, afd->transicoes[i]);
        char *origem = strtok(temp, " ");
        char *simb = strtok(NULL, " ");
        char *dest = strtok(NULL, " ");

        if (strcmp(origem, afd->estados[estado_idx]) == 0 && strcmp(simb, simbolo) == 0) {
            return buscarIndice(afd, dest);
        }
    }
    return -1;
}

void preencherMAFD(AFD *afd, MAFD *mafd, int grupo[], int qtd_vivos, int mapa[]) {
    int max_grupo = -1;
    for (int i = 0; i < qtd_vivos; i++) {
        if (grupo[i] > max_grupo) max_grupo = grupo[i];
    }
    mafd->qtd_estados = max_grupo + 1;

    // 1. Definir os nomes dos novos estados (G0, G1, etc)
    for (int i = 0; i < mafd->qtd_estados; i++) {
        sprintf(mafd->estados[i], "s%d", i);
    }

    // 2. Definir o Alfabeto (é o mesmo do original)
    mafd->qtd_alfabeto = afd->qtd_alfabeto;
    for (int i = 0; i < afd->qtd_alfabeto; i++) {
        strcpy(mafd->alfabeto[i], afd->alfabeto[i]);
    }

    // 3. Definir o novo Estado Inicial
    int idx_inicial_orig = buscarIndice(afd, afd->estado_inicial);
    for (int i = 0; i < qtd_vivos; i++) {
        if (mapa[i] == idx_inicial_orig) {
            sprintf(mafd->estado_inicial, "s%d", grupo[i]);
            break;
        }
    }

    // 4. Definir os novos Estados Finais (sem repetir)
    mafd->qtd_finais = 0;
    bool grupo_ja_eh_final[MAX_ESTADOS] = {false};

    for (int i = 0; i < qtd_vivos; i++) {
        int idx_orig = mapa[i];
        bool eh_final = false;
        for (int f = 0; f < afd->qtd_finais; f++) {
            if (strcmp(afd->estados[idx_orig], afd->estados_finais[f]) == 0) {
                eh_final = true;
                break;
            }
        }

        if (eh_final && !grupo_ja_eh_final[grupo[i]]) {
            strcpy(mafd->estados_finais[mafd->qtd_finais], mafd->estados[grupo[i]]);
            grupo_ja_eh_final[grupo[i]] = true;
            mafd->qtd_finais++;
        }
    }

    // 5. Definir as novas Transições
    mafd->qtd_transicoes = 0;
    for (int g = 0; g < mafd->qtd_estados; g++) {
        // Pega o primeiro estado que pertence a este grupo para ver para onde ele vai
        int representante = -1;
        for (int i = 0; i < qtd_vivos; i++) {
            if (grupo[i] == g) {
                representante = i;
                break;
            }
        }

        for (int s = 0; s < afd->qtd_alfabeto; s++) {
            int dest_orig = destinoTransicao(afd, mapa[representante], afd->alfabeto[s]);
            
            // Descobre a qual grupo esse destino pertence
            int grupo_dest = -1;
            for (int k = 0; k < qtd_vivos; k++) {
                if (mapa[k] == dest_orig) {
                    grupo_dest = grupo[k];
                    break;
                }
            }

            if (grupo_dest != -1) {
                sprintf(mafd->transicoes[mafd->qtd_transicoes], "%s %s %s", 
                        mafd->estados[g], afd->alfabeto[s], mafd->estados[grupo_dest]);
                mafd->qtd_transicoes++;
            }
        }
    }
}

void minimizarAfd(AFD *afd, MAFD *mafd) {
    int grupo[MAX_ESTADOS];
    int novo_grupo[MAX_ESTADOS];
    int qtd_estados_vivos = 0;
    int mapa_antigo_novo[MAX_ESTADOS];

    // 1. Filtrar apenas os alcancaveis para trabalhar
    for (int i = 0; i < afd->qtd_estados; i++) {
        if (afd->alcancaveis[i]) {
            mapa_antigo_novo[qtd_estados_vivos] = i;
            qtd_estados_vivos++;
        }
    }

    // 2. Partição Inicial: Finais (1) vs Não-Finais (0)
    for (int i = 0; i < qtd_estados_vivos; i++) {
        int idx_original = mapa_antigo_novo[i];
        grupo[i] = 0; 
        for (int j = 0; j < afd->qtd_finais; j++) {
            if (strcmp(afd->estados[idx_original], afd->estados_finais[j]) == 0) {
                grupo[i] = 1;
                break;
            }
        }
    }

    // 3. O Loop "Mudou" (Refinamento)
    bool mudou = true;
    while (mudou) {
        mudou = false;
        int proximo_id_grupo = 0;
        
        for (int i = 0; i < qtd_estados_vivos; i++) {
            novo_grupo[i] = -1; // Reset para processar
        }

        for (int i = 0; i < qtd_estados_vivos; i++) {
            if (novo_grupo[i] != -1) continue; // Já agrupado nesta rodada

            novo_grupo[i] = proximo_id_grupo;
            
            for (int j = i + 1; j < qtd_estados_vivos; j++) {
                if (novo_grupo[j] != -1) continue;

                // Teste de Equivalência:
                // Estão no mesmo grupo atual?
                bool equivalentes = (grupo[i] == grupo[j]);
                
                if (equivalentes) {
                    // Para cada símbolo do alfabeto, eles levam a grupos iguais?
                    for (int s = 0; s < afd->qtd_alfabeto; s++) {
                        int dest_i = destinoTransicao(afd, mapa_antigo_novo[i], afd->alfabeto[s]);
                        int dest_j = destinoTransicao(afd, mapa_antigo_novo[j], afd->alfabeto[s]);
                        
                        // Encontra em qual índice da nossa lista "viva" esses destinos estão
                        int g_i = -1, g_j = -1;
                        for(int k=0; k<qtd_estados_vivos; k++) {
                            if (mapa_antigo_novo[k] == dest_i) g_i = grupo[k];
                            if (mapa_antigo_novo[k] == dest_j) g_j = grupo[k];
                        }

                        if (g_i != g_j) {
                            equivalentes = false;
                            break;
                        }
                    }
                }

                if (equivalentes) {
                    novo_grupo[j] = proximo_id_grupo;
                }
            }
            proximo_id_grupo++;
        }

        // Verifica se o número de grupos aumentou (se alguém foi separado)
        for (int i = 0; i < qtd_estados_vivos; i++) {
            if (grupo[i] != novo_grupo[i]) {
                mudou = true;
            }
            grupo[i] = novo_grupo[i];
        }
    }

    // 4. Montar o MAFD baseado nos grupos resultantes
    // (Aqui você traduz os grupos de volta para a struct MAFD)
    preencherMAFD(afd, mafd, grupo, qtd_estados_vivos, mapa_antigo_novo);
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
    for(int i=0; i < mafd->qtd_alfabeto; i++) fprintf(file, " %s", mafd->alfabeto[i]);    

    fprintf(file, "\n# Estados\n");
    fprintf(file, "E %s\n", mafd->estados);

    fprintf(file, "\n# Estado Inicial\n");
    fprintf(file, "\nq %s", mafd->estado_inicial);

    fprintf(file, "\n# Estados Finais\n");
    for(int i=0; i < mafd->qtd_finais; i++) fprintf(file, "\n%s\n", mafd->estados_finais[i]);

    fprintf(file, "\n# Transições\n");
    for(int i=0; i < mafd->qtd_transicoes; i++) fprintf(file, "T %s\n", mafd->transicoes[i]);

    fclose(file);
}

int main(){
    ListaDeLinhas entrada;
    AFD afd;
    MAFD mafd;
    const char *file = "saida.txt";
    carregarArquivo("e1.txt", &entrada);
    processararAFD(&entrada, &afd);
    removerEstadosInacessiveis(&afd);
    // printf("Estados alcancados:\n");
    // for (int i = 0; i < afd.qtd_estados; i++) {
    //     if (afd.alcancaveis[i]) printf("- %s\n", afd.estados[i]);
    // }
    minimizarAfd(&afd, &mafd);
    saida(&mafd, file);
    return 0;
}
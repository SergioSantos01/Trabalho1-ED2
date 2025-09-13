#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_INICIAL 101
#define MAX_DESCRICAO 60
#define MAX_CODIGO 20

typedef struct No {
    char codigo[MAX_CODIGO];
    char descricao[MAX_DESCRICAO];
    int quantidade;
    float preco;
    struct No* prox;
} No;

typedef struct {
    No** tabela;
    int tamanho;
    int num_elementos;
    int (*funcao_hash)(char*, int);
} TabelaHash;

// Função para converter código alfanumérico para decimal usando valores ASCII
long long converter_para_decimal(char* codigo) {
    long long decimal = 0;
    int i;
    for (i = 0; codigo[i] != '\0'; i++) {
        decimal = decimal * 128 + (int)codigo[i];
    }
    return decimal;
}

// Função hash usando método da divisão
int hash_divisao(char* codigo, int tamanho_tabela) {
    long long decimal = converter_para_decimal(codigo);
    return (int)(decimal % tamanho_tabela);
}

// Método da Multiplicação
int hash_multiplicacao(char* codigo, int tamanho_tabela) {
    long long decimal = converter_para_decimal(codigo);
    double A = 0.618; // Constante áurea (fracionária)
    
    double valor = decimal * A;
    valor = valor - (long long)valor; // Parte fracionária
    
    return (int)(tamanho_tabela * valor);
}

// Método de Dobras
int hash_dobras(char* codigo, int tamanho_tabela) {
    long long decimal = converter_para_decimal(codigo);
    int soma = 0;
    
    // Dividir o número em partes de 2 dígitos e somar
    while (decimal > 0) {
        soma += decimal % 100; // Pega últimos 2 dígitos
        decimal /= 100;        // Remove os 2 dígitos
    }
    
    return soma % tamanho_tabela;
}

// Inicializar a tabela hash
void inicializar_tabela(TabelaHash* th, int tamanho, int (*funcao_hash)(char*, int)) {
    th->tabela = (No**)malloc(tamanho * sizeof(No*));
    th->tamanho = tamanho;
    th->num_elementos = 0;
    th->funcao_hash = funcao_hash;
    int i;
    
    for (i = 0; i < tamanho; i++) {
        th->tabela[i] = NULL;
    }
}

// Função para buscar uma peça pelo código
void buscar_peca(TabelaHash* th) {
    char codigo[MAX_CODIGO];
    
    // Limpar buffer do teclado
    while (getchar() != '\n');
    
    printf("\n [2] Buscar Peca por Codigo\n");
    printf("codigo: ");
    
    if (fgets(codigo, MAX_CODIGO, stdin) == NULL) {
        printf("Erro na leitura do codigo.\n");
        return;
    }
    codigo[strcspn(codigo, "\n")] = '\0'; // Remover quebra de linha
    
    int indice = th->funcao_hash(codigo, th->tamanho);
    No* atual = th->tabela[indice];
    int encontrada = 0;
    
    while (atual != NULL) {
        if (strcmp(atual->codigo, codigo) == 0) {
            printf("\nPeca encontrada:\n");
            printf("Codigo: %s\n", atual->codigo);
            printf("Descricao: %s\n", atual->descricao);
            printf("Quantidade: %d\n", atual->quantidade);
            printf("Preco: %.2f\n\n", atual->preco);
            encontrada = 1;
            break;
        }
        atual = atual->prox;
    }
    
    if (!encontrada) {
        printf("Nenhuma peca encontrada com o codigo informado.\n\n");
    }
}

// Função auxiliar para buscar um nó pelo código 
No* buscar_no_por_codigo(TabelaHash* th, char* codigo) {
    int indice = th->funcao_hash(codigo, th->tamanho);
    No* atual = th->tabela[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->codigo, codigo) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    
    return NULL;
}

int eh_primo(int n) {
    int i;
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    
    for (i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

int proximo_primo(int n) {
    while (!eh_primo(n)) n++;
    return n;
}

void fazer_rehash(TabelaHash* th, int novoTamanho) {
    // 1. Criar nova tabela temporária
    No** novaTabela = (No**)malloc(novoTamanho * sizeof(No*));
    int i;
    for (i = 0; i < novoTamanho; i++) {
        novaTabela[i] = NULL;
    }
    
    int elementos_reinseridos = 0;
    
    // 2. Reinserir todos os elementos na nova tabela
    int l;
    for (l = 0; l < th->tamanho; l++) {
        No* atual = th->tabela[l];
        while (atual != NULL) {
            No* proximo = atual->prox;
            
            // Recalcular hash com novo tamanho
            int novoIndice = th->funcao_hash(atual->codigo, novoTamanho);
            
            // Inserir na nova tabela
            atual->prox = novaTabela[novoIndice];
            novaTabela[novoIndice] = atual;
            
            elementos_reinseridos++;
            atual = proximo;
        }
    }
    
    // 3. Substituir tabela antiga pela nova
    free(th->tabela);
    th->tabela = novaTabela;
    th->tamanho = novoTamanho;
    
    printf("Rehash realizado: %d elementos, novo tamanho: %d\n", elementos_reinseridos, novoTamanho);
}

void verificar_rehash(TabelaHash* th) {
    float fator_carga = (float)th->num_elementos / th->tamanho;
    
    if (fator_carga > 0.75) {
        int novoTamanho = proximo_primo(th->tamanho * 2);
        printf("Fator de carga (%.3f) > 0.75. Fazendo rehash para tamanho %d...\n", 
               fator_carga, novoTamanho);
        fazer_rehash(th, novoTamanho);
    }
}

// Função de inserir nova peca
void inserir_peca(TabelaHash* th) {
    char codigo[MAX_CODIGO];
    char descricao[MAX_DESCRICAO];
    int quantidade;
    float preco;
    
    // Limpar buffer do teclado
    while (getchar() != '\n');
    
    printf("\n[1] Inserir Nova Peca \n");
    
    // Capturar código
    printf("codigo: ");
    if (fgets(codigo, MAX_CODIGO, stdin) == NULL) {
        printf("Erro na leitura do codigo.\n");
        return;
    }
    codigo[strcspn(codigo, "\n")] = '\0'; // Remover quebra de linha
    
    // Verificar se já existe uma peça com este código (usando a função auxiliar)
    if (buscar_no_por_codigo(th, codigo) != NULL) {
        printf("Erro: ja existe uma peca cadastrada com o codigo %s.\n\n", codigo);
        return;
    }
    
    // Capturar descrição
    printf("descricao: ");
    if (fgets(descricao, MAX_DESCRICAO, stdin) == NULL) {
        printf("Erro na leitura da descricao.\n");
        return;
    }
    descricao[strcspn(descricao, "\n")] = '\0'; // Remover quebra de linha
    
    // Capturar quantidade
    printf("qtde: ");
    if (scanf("%d", &quantidade) != 1 || quantidade < 0) {
        printf("Quantidade invalida.\n");
        while (getchar() != '\n'); // Limpar buffer
        return;
    }
    
    // Capturar preço
    printf("preco: ");
    if (scanf("%f", &preco) != 1 || preco <= 0) {
        printf("Preco invalido.\n");
        while (getchar() != '\n'); // Limpar buffer
        return;
    }
    
    // Limpar buffer após os scanfs
    while (getchar() != '\n');
    
    // Criar nova peça
    No* nova_peca = (No*)malloc(sizeof(No));
    strcpy(nova_peca->codigo, codigo);
    strcpy(nova_peca->descricao, descricao);
    nova_peca->quantidade = quantidade;
    nova_peca->preco = preco;
    
    // Calcular índice usando função hash
    int indice = th->funcao_hash(codigo, th->tamanho);
    
    // Inserir na tabela (encadeamento separado)
    nova_peca->prox = th->tabela[indice];
    th->tabela[indice] = nova_peca;
    th->num_elementos++;
    
    printf("Peca %s inserida com sucesso.\n\n", codigo);

    verificar_rehash(th);
}

void remover_peca(TabelaHash* th) {
    char codigo[MAX_CODIGO];
    
    // Limpar buffer do teclado
    while (getchar() != '\n');
    
    printf("\n[3] Remover Peca do Estoque\n");
    printf("codigo: ");
    
    if (fgets(codigo, MAX_CODIGO, stdin) == NULL) {
        printf("Erro na leitura do codigo.\n");
        return;
    }
    codigo[strcspn(codigo, "\n")] = '\0'; // Remover quebra de linha
    
    int indice = th->funcao_hash(codigo, th->tamanho);
    No* atual = th->tabela[indice];
    No* anterior = NULL;
    int encontrada = 0;
    
    while (atual != NULL) {
        if (strcmp(atual->codigo, codigo) == 0) {
            if (anterior == NULL) {
                // Remove o primeiro nó da lista
                th->tabela[indice] = atual->prox;
            } else {
                // Remove um nó do meio/final da lista
                anterior->prox = atual->prox;
            }
            
            free(atual);
            th->num_elementos--;
            printf("Peca %s removida com sucesso.\n\n", codigo);
            encontrada = 1;
            break;
        }
        anterior = atual;
        atual = atual->prox;
    }
    
    if (!encontrada) {
        printf("Erro: nao foi encontrada peca com o codigo %s.\n\n", codigo);
    }
}

void exibir_estatisticas(TabelaHash* th) {
    printf("\n[4] Exibir Estatisticas da Tabela\n");
    
    int buckets_utilizados = 0;
    int maior_lista = 0;
    int total_colisoes = 0;
    
    // Calcular estatísticas
    int i;
    for (i = 0; i < th->tamanho; i++) {
        int tamanho_lista = 0;
        No* atual = th->tabela[i];
        
        if (atual != NULL) {
            buckets_utilizados++;
        }
        
        while (atual != NULL) {
            tamanho_lista++;
            atual = atual->prox;
        }
        
        if (tamanho_lista > maior_lista) {
            maior_lista = tamanho_lista;
        }
        
        // Contar colisões: se a lista tem mais de 1 elemento, há colisões
        if (tamanho_lista > 1) {
            total_colisoes += (tamanho_lista - 1);
        }
    }
    
    float fator_carga = (float)th->num_elementos / th->tamanho;
    float percentual_utilizados = (float)buckets_utilizados / th->tamanho * 100;
    
    // Exibir estatísticas
    printf("Tamanho da tabela (m): %d\n", th->tamanho);
    printf("Numero de itens (n): %d\n", th->num_elementos);
    printf("Fator de carga ($\\alpha$): %.3f\n", fator_carga);
    printf("Buckets utilizados: %d (%.2f%%)\n", buckets_utilizados, percentual_utilizados);
    printf("Total de colisoes: %d\n", total_colisoes);
    
    if (maior_lista > 0) {
        printf("Maior lista (tamanho): %d elementos\n", maior_lista);
    } else {
        printf("Maior lista (tamanho): 0 elementos\n");
    }

}

void carregar_csv(TabelaHash* th) {
    char nome_arquivo[100];
    char caminho_completo[256];
    int itens_carregados = 0;
    int linhas_processadas = 0;
    
    // Limpar buffer do teclado
    while (getchar() != '\n');
    
    printf("\n[5] Carregar Pecas de um Arquivo CSV\n");
    printf("nome do arquivo: ");
    
    if (fgets(nome_arquivo, sizeof(nome_arquivo), stdin) == NULL) {
        printf("Erro na leitura do nome do arquivo.\n");
        return;
    }
    nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
    
    // Construir caminho completo
    snprintf(caminho_completo, sizeof(caminho_completo), "../entrada_csv/%s", nome_arquivo);
    
    FILE* arquivo = fopen(caminho_completo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n\n", caminho_completo);
        return;
    }
    
    // Pular cabeçalho (primeira linha)
    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        printf("Arquivo vazio ou erro na leitura.\n");
        fclose(arquivo);
        return;
    }
    
    // Ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        linhas_processadas++;
        
        // Remover quebra de linha
        linha[strcspn(linha, "\n")] = '\0';
        
        // Pular linha vazia
        if (strlen(linha) == 0) {
            continue;
        }
        
        char codigo[MAX_CODIGO];
        char descricao[MAX_DESCRICAO];
        int qtde;
        float preco;
        
        // Parse da linha CSV (formato: codigo;descricao;qtde;preco)
        if (sscanf(linha, "%[^;];%[^;];%d;%f", 
                  codigo, descricao, &qtde, &preco) == 4) {
            
            // Verificar se já existe
            if (buscar_no_por_codigo(th, codigo) == NULL) {
                // Criar nova peça
                No* nova_peca = (No*)malloc(sizeof(No));
                strcpy(nova_peca->codigo, codigo);
                strcpy(nova_peca->descricao, descricao);
                nova_peca->quantidade = qtde;
                nova_peca->preco = preco;
                
                // Inserir na tabela
                int indice = th->funcao_hash(codigo, th->tamanho);
                nova_peca->prox = th->tabela[indice];
                th->tabela[indice] = nova_peca;
                th->num_elementos++;
                
                itens_carregados++;
            }
        } else {
            printf("Erro no formato da linha %d: %s\n", linhas_processadas, linha);
        }
    }
    
    fclose(arquivo);
    printf("%d itens carregados de %s.\n\n", itens_carregados, caminho_completo);

    verificar_rehash(th);
}

void salvar_csv(TabelaHash* th) {
    char nome_arquivo[100];
    char caminho_completo[256];
    int itens_salvos = 0;
    
    // Limpar buffer do teclado
    while (getchar() != '\n');
    
    printf("\n[6] Salvar Tabela em Arquivo CSV\n");
    printf("arquivo: ");
    
    if (fgets(nome_arquivo, sizeof(nome_arquivo), stdin) == NULL) {
        printf("Erro na leitura do nome do arquivo.\n");
        return;
    }
    nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
    
    // Construir caminho completo para saida_csv
    snprintf(caminho_completo, sizeof(caminho_completo), "../saida_csv/%s", nome_arquivo);
    
    FILE* arquivo = fopen(caminho_completo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo %s.\n\n", caminho_completo);
        return;
    }
    
    // Escrever cabeçalho
    fprintf(arquivo, "codigo;descricao;qtde;preco\n");
    
    // Escrever todos os itens da tabela
    int i;
    for (i = 0; i < th->tamanho; i++) {
        No* atual = th->tabela[i];
        while (atual != NULL) {
            fprintf(arquivo, "%s;%s;%d;%.2f\n", 
                   atual->codigo, 
                   atual->descricao, 
                   atual->quantidade, 
                   atual->preco);
            itens_salvos++;
            atual = atual->prox;
        }
    }
    
    fclose(arquivo);
    printf("Tabela salva em %s (%d itens).\n\n", caminho_completo, itens_salvos);
}

void trocar_funcao_hash(TabelaHash* th) {
    int opcao;
    int (*nova_funcao)(char*, int) = NULL;
    char* nome_funcao = "";
    
    printf("\n[7] Trocar Funcao de Hash\n");
    printf("Selecione o metodo:\n");
    printf("[1] Divisao (Padrao)\n");
    printf("[2] Multiplicacao\n");
    printf("[3] Dobras\n");
    printf("Opcao: ");
    
    if (scanf("%d", &opcao) != 1) {
        printf("Opcao invalida.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n'); // Limpar buffer
    
    switch (opcao) {
        case 1:
            nova_funcao = hash_divisao;
            nome_funcao = "Divisao";
            break;
        case 2:
            nova_funcao = hash_multiplicacao;
            nome_funcao = "Multiplicacao";
            break;
        case 3:
            nova_funcao = hash_dobras;
            nome_funcao = "Dobras";
            break;
        default:
            printf("Opcao invalida.\n");
            return;
    }
    
    // Aplicar nova função hash
    th->funcao_hash = nova_funcao;
    
    // Fazer rehash para recalcular todas as posições
    printf("Funcao hash alterada para %s.\n", nome_funcao);
    printf("Rehash automatico realizado. Novo tamanho da tabela: %d.\n\n", th->tamanho);
    
    // REHASH FORÇADO - mantém mesmo tamanho mas recalcula tudo
    fazer_rehash(th, th->tamanho);
}

// Liberar memória da tabela hash
void liberar_tabela(TabelaHash* th) {
    int i;
    for (i = 0; i < th->tamanho; i++) {
        No* atual = th->tabela[i];
        while (atual != NULL) {
            No* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(th->tabela);
}

int main(void) {
    int opcao = 0;
    TabelaHash th;
    
    // Inicializar tabela hash com método da divisão
    inicializar_tabela(&th, TAMANHO_INICIAL, hash_divisao);

    do {
        printf("===========================================\n");
        printf("SISTEMA DE ESTOQUE - TECHPARTS\n");
        printf("===========================================\n\n");
        printf("[1] Inserir nova peca\n");
        printf("[2] Buscar peca por codigo\n");
        printf("[3] Remover peca do estoque\n");
        printf("[4] Exibir estatisticas da tabela\n");
        printf("[5] Carregar pecas de um arquivo CSV\n");
        printf("[6] Salvar tabela em arquivo CSV\n");
        printf("[7] Trocar funcao de hash (Divisao (Por Padrao) <-> Multiplicacao <-> Dobra)\n");
        printf("[8] Encerrar o programa\n\n");
        printf("Digite a opcao desejada: ");

        if (scanf("%d", &opcao) != 1) {
            fprintf(stderr, "Entrada invalida.\n");
            while (getchar() != '\n'); // Limpar buffer
            continue;
        }

        switch (opcao) {
            case 1: 
                inserir_peca(&th);
                break;
            case 2:
                buscar_peca(&th);
                break;
            case 3: 
                remover_peca(&th); 
                break;
            case 4:
                exibir_estatisticas(&th);
                break;
            case 5:
                carregar_csv(&th);
                break;
            case 6:
                salvar_csv(&th);
                break;
            case 7:
                trocar_funcao_hash(&th); 
                break;
            case 8: 
                printf("Encerrando...\n"); 
                break;
            default: 
                printf("Opcao invalida.\n");
        }
    } while (opcao != 8);

    liberar_tabela(&th);
    return 0;
}
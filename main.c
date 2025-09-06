#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int opcao = 0;

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
        printf("[7] Trocar funcao de hash (Divisao <-> Multiplicacao)\n");
        printf("[8] Encerrar o programa\n\n");
        printf("Digite a opcao desejada: ");

        if (scanf("%d", &opcao) != 1) {
            fprintf(stderr, "Entrada invalida.\n");
            break;
        }

        switch (opcao) {
            case 1: /* inserir */ break;
            case 2: /* buscar */ break;
            case 3: /* remover */ break;
            case 4: /* estatisticas */ break;
            case 5: /* carregar CSV */ break;
            case 6: /* salvar CSV */ break;
            case 7: /* alternar hash + rehash */ break;
            case 8: printf("Encerrando...\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 8);

    /* liberar recursos */
    return 0;
}
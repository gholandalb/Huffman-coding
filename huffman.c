#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 256

typedef struct No{
    unsigned char byte;
    unsigned int frequencia;
    struct No *esq, *dir, *prox;
}No;

typedef struct {
    No *inicio;
    int tam;
} Lista;


// ------tabela de frequencia-------

void inicializa_tabela_com_0(unsigned int tab[])
{
    for (int i = 0; i < TAM; i++)
        tab[i] = 0;
}

void preenche_tab_frequencia(FILE *arquivo, unsigned int tab[])
{
    int byte;
    while((byte = fgetc(arquivo)) != EOF)
    {
        tab[(unsigned char)byte]++;
    }
}

void imprime_tab_freq(unsigned int tab[])
{
    printf("------Tabela de frequencia------\n");
    for (int i = 0; i < TAM; i++)
    {
        if (tab[i] != 0)
        printf("Byte: 0x%02X      Frequencia: %u\n", i, tab[i], i);
    }

}

// Lista encadeada ordenada

void criar_Lista(Lista *lista)
{
    lista->inicio = NULL;
    lista->tam = 0;
} 

void inserir_ordenadamente(Lista *lista, No *no)
{
    No *aux;
    no->prox = NULL;

    if (lista->inicio == NULL) //qnd a lista eh vazia
    {
        lista->inicio = no;
    }
    else if (no->frequencia < lista->inicio->frequencia) //verifica se a frequencia eh menor q o primeiro elemento da lista
    {
        no->prox = lista->inicio;
        lista->inicio = no;
    } 
    else //caso quando vai ser inserido no meio ou final
    {
        aux = lista->inicio;
        while (aux->prox != NULL && aux->prox->frequencia <= no->frequencia)
            aux = aux->prox;
        no->prox = aux->prox;
        aux->prox = no;
    }
    lista->tam++;
}

void preencher_lista(unsigned int tab[], Lista *lista)
{
    No *novo;
    for (int i = 0; i < TAM; i++)
    {
        if (tab[i] > 0)
        {
            novo = malloc(sizeof(No));
            if (novo)
            {
                novo->byte = i;
                novo->frequencia = tab[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo->prox = NULL;

                inserir_ordenadamente(lista, novo);
            }
            else
            {
                printf("Erro na alocação de memória em preencher_lista!");
                break;
            }
        }
    }
}

void imprimir_lista(Lista *lista)
{
    No *aux = lista->inicio;
    printf("------Lista ordenada, tamanho: %d------\n", lista->tam);
    while (aux != NULL)
    {
        printf("Byte: 0x%02X       Frequencia: %u       char: %c\n", aux->byte, aux->frequencia, aux->byte);
        aux = aux->prox;
    }
}


// Montar arvore de huffman

void *remover_no_inicio(Lista *lista)
{
    No *aux = NULL;
    if (lista->inicio != NULL)
    {
        aux = lista->inicio;
        lista->inicio = aux->prox;
        aux->prox = NULL;
        lista->tam--;
    }
    return aux;
}

void *montar_arvore(Lista *lista)
{
    No *primeiro, *segundo, *novo;
    while (lista->tam > 1)
    {
        primeiro = remover_no_inicio(lista);
        segundo = remover_no_inicio(lista);
        novo = malloc(sizeof(No));

        if (novo)
        {
            novo->byte = '+';
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->prox = NULL;

            inserir_ordenadamente(lista, novo);
        }
        else
        {
            printf("erro ao alocar memoria em montar_arvore");
            break;
        }
    }
    return lista->inicio;
}

void imprimir_arvore(No *raiz, int tam)
{
    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        printf("Folha: 0x%02X      Altura: %d      char: %c\n", raiz->byte, tam, raiz->byte);
    }
    else
    {
        imprimir_arvore(raiz->esq, tam + 1);
        imprimir_arvore(raiz->dir, tam + 1);
    }
}


//montar dicionario

int altura_arvore(No *raiz)
{
    int esq, dir;
    if (raiz == NULL)
        return -1;
    else
    {
        esq = altura_arvore(raiz->esq) + 1;
        dir = altura_arvore(raiz->dir) + 1;

        if (esq > dir)
            return esq;
        else
            return dir;
    }
}

char** alocar_dicionario(int colunas)
{
    char **dicionario;

    dicionario = malloc(sizeof(char*) * TAM);
    for (int i = 0; i < TAM; i++)
        dicionario[i] = calloc(colunas, sizeof(char));
    return dicionario;
} 

void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas)
{
    char esquerda[colunas], direita[colunas];

    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        strcpy(dicionario[raiz->byte], caminho);
    }
    else
    {
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_dicionario(dicionario, raiz->esq, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->dir, direita, colunas);
    }
}

void imprimir_dicionario(char **dicionario)
{
    printf("------dicionario------\n");
    for (int i = 0; i < TAM; i++)
    {
        if (dicionario[i][0] != '\0')                   
            printf("0x%02X: %s\n", i, dicionario[i]);
    }
}


int main()
{
    unsigned int tabela_frequencia[TAM];
    char nome_arquivo[256];

    Lista lista;
    No *arvore;
    int colunas;
    char **dicionario;

    printf("Digite o nome do arquivo que vai ser compactado: ");
    scanf("%s", nome_arquivo);

    FILE *arquivo = fopen(nome_arquivo, "rb");

        if (arquivo == NULL)
        {
            printf("Erro ao abrir o arquivo.\n");
            return 1;
        }


    // tabela de frequencia

    inicializa_tabela_com_0(tabela_frequencia);
    preenche_tab_frequencia(arquivo, tabela_frequencia);
    fclose(arquivo);

    imprime_tab_freq(tabela_frequencia);


    // lista encadeada
    criar_Lista(&lista);
    preencher_lista(tabela_frequencia, &lista);
    imprimir_lista(&lista);

    // arvore de huffman
    arvore = montar_arvore(&lista);
    printf("------Arvore de huffman:------\n");
    imprimir_arvore(arvore, 0);

    //montar dicionario
    colunas = altura_arvore(arvore) + 1;
    dicionario = alocar_dicionario(colunas);
    gerar_dicionario(dicionario, arvore, "", colunas);
    imprimir_dicionario(dicionario);

    return 0;
}
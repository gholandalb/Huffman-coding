#include <stdio.h>
#include <stdlib.h>

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
        printf("Byte: 0x%02X, Frequencia: %u\n", i, tab[i], i);
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
        printf("Byte: 0x%02X, Frequencia: %u, char(se possivel): %c\n", aux->byte, aux->frequencia, aux->byte);
        aux = aux->prox;
    }
}

int main()
{
    unsigned int tabela_frequencia[TAM];
    char nome_arquivo[256];

    Lista lista;

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


    return 0;
}
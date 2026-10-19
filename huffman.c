#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 256

typedef struct No{
    void *dado;
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
        printf("Byte: 0x%02X      Frequencia: %u\n", i, tab[i]);
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
    unsigned char *byte_alocado;
    for (int i = 0; i < TAM; i++)
    {
        if (tab[i] > 0)
        {
            novo = malloc(sizeof(No));
            byte_alocado = malloc(sizeof(unsigned char));
            if (novo && byte_alocado)
            {
                *byte_alocado = i;
                novo->dado = byte_alocado;
                novo->frequencia = tab[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo->prox = NULL;

                inserir_ordenadamente(lista, novo);
            }
            else
            {
                free(novo);
                free(byte_alocado);
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
        unsigned char byte_do_no = *(unsigned char *)aux->dado;
        printf("Byte: 0x%02X       Frequencia: %u       char: %c\n", byte_do_no, aux->frequencia, byte_do_no);
        aux = aux->prox;
    }
}


// Montar arvore de huffman

No *remover_no_inicio(Lista *lista)
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

No *montar_arvore(Lista *lista)
{
    No *primeiro, *segundo, *novo;
    unsigned char *byte_no_interno;
    while (lista->tam > 1)
    {
        primeiro = remover_no_inicio(lista);
        segundo = remover_no_inicio(lista);
        novo = malloc(sizeof(No));
        byte_no_interno = malloc(sizeof(unsigned char));

        if (novo && byte_no_interno)
        {
            *byte_no_interno = '+';
            novo->dado = byte_no_interno;
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->prox = NULL;

            inserir_ordenadamente(lista, novo);
        }
        else
        {
            free(novo);
            free(byte_no_interno);
            printf("erro ao alocar memoria em montar_arvore");
            break;
        }
    }
    return lista->inicio;
}

No *construir_arvore(unsigned int tab[])
{
    Lista lista;
    criar_Lista(&lista);
    preencher_lista(tab, &lista);
    return montar_arvore(&lista); // lista vazia -> lista.inicio == NULL -> retorna NULL
}

void imprimir_arvore(No *raiz, int tam)
{
    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        unsigned char byte_da_folha = *(unsigned char *)raiz->dado;
        printf("Folha: 0x%02X      Altura: %d      char: %c\n", byte_da_folha, tam, byte_da_folha);
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
        unsigned char byte_da_folha = *(unsigned char *)raiz->dado;

        if (caminho[0] == '\0')
            strcpy(dicionario[byte_da_folha], "0");
        else
            strcpy(dicionario[byte_da_folha], caminho);
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

// codificar 

unsigned long long calcular_tamanho_file(char **dicionario, FILE *arquivo)
{
    unsigned long long tam = 0;
    int byte;
    while((byte = fgetc(arquivo)) != EOF)
    {
        tam += strlen(dicionario[(unsigned char)byte]);
    }
    return tam + 1;
}


char* codificar(char **dicionario, FILE *arquivo)
{
    rewind(arquivo);  
    unsigned long long tam = calcular_tamanho_file(dicionario, arquivo);
    rewind(arquivo);

    char *codigo = calloc(tam, sizeof(char));
    if (codigo == NULL)
        return NULL;

    int byte;
    while((byte = fgetc(arquivo)) != EOF)
    {
        strcat(codigo, dicionario[(unsigned char)byte]);
    }
    return codigo;
}
//esta funcao (versao em string) NAO eh mais usada no fluxo principal, pois nao
// funciona para arquivos binarios (bytes 0x00 terminam a string). Mantida so como
// demonstracao do tutorial, apenas para textos. A descompactacao real eh a funcao
// descompactar(), mais abaixo.

char *decodificar(char texto[], No *raiz)
{
    int i = 0;
    int j = 0; 
    No *aux = raiz;
    char *decodificado = calloc(strlen(texto) + 1, sizeof(char));
    if (decodificado == NULL) 
        return NULL;

    while (texto[i] != '\0')
    {
        if (texto[i] == '0')
            aux = aux->esq;
        else
            aux = aux->dir;

        if (aux->esq == NULL && aux->dir == NULL)
        {
            decodificado[j++] = *(unsigned char *)aux->dado;
            aux = raiz;
        }
        i++;
    }
    return decodificado;
}

//compactar

void compactar(unsigned char str[], unsigned tab[], unsigned long long total_bytes, char nome_saida[])
{
    FILE *arquivo = fopen(nome_saida, "wb");
    int i = 0, j = 7;
    unsigned char mascara, byte = 0;
    if (arquivo)
    {
        // Cabecalho------------------------
        //tamanho original do arquivo (para saber quando parar ao descompactar
        fwrite(&total_bytes, sizeof(unsigned long long), 1, arquivo);

        // quantos bytes distintos existem (de 1 a 256, cabe em unsigned short)
        unsigned short qtd = 0;
        for (int k = 0; k < TAM; k++)
            if (tab[k] > 0)
                qtd++;
        fwrite(&qtd, sizeof(unsigned short), 1, arquivo);

        //cada par (byte, frequencia) com isso o descompactador reconstroi a arvore
        for (int k = 0; k < TAM; k++)
        {
            if (tab[k] > 0)
            {
                unsigned char b = (unsigned char)k;
                fwrite(&b, sizeof(unsigned char), 1, arquivo);
                fwrite(&tab[k], sizeof(unsigned int), 1, arquivo);
            }
        }

        while(str[i] != '\0')
        {
            mascara = 1;
            if (str[i] == '1')
            {
                mascara = mascara << j;
                byte = byte | mascara;
            }
            j--;
            if (j < 0)
            {
                fwrite(&byte, sizeof(unsigned char), 1, arquivo);
                byte = 0;
                j = 7;
            }
            i++;
        }
        if (j != 7)
        {
            fwrite(&byte, sizeof(unsigned char), 1, arquivo);
        }
        fclose(arquivo);
    }
    else
        printf("erro ao abrir arquivo em compactar\n");
}

// descompactar

unsigned int eh_bit_um(unsigned char byte, int i)
{
    unsigned char mascara = (1 << i);
    return mascara & byte;
}

//a funcao faz tudo sozinha: le o cabecalho, reconstroi a arvore e grava o resultado num arquivo. Assim funciona com qualquer .gh, mesmo de outra execucao.

void descompactar(char nome_entrada[], char nome_saida[])
{
    FILE *entrada = fopen(nome_entrada, "rb");
    if (entrada == NULL)
    {
        printf("erro ao abrir arquivo em descompactar\n");
        return;
    }


    // le o cabecalho gravado por compactar()

    unsigned long long total_bytes;
    unsigned short qtd;
    unsigned int tabela[TAM];
    inicializa_tabela_com_0(tabela);

    if (fread(&total_bytes, sizeof(unsigned long long), 1, entrada) != 1 || fread(&qtd, sizeof(unsigned short), 1, entrada) != 1 || qtd == 0 || qtd > TAM)
    {
        printf("arquivo compactado invalido (cabecalho corrompido)\n");
        fclose(entrada);
        return;
    }

    for (int k = 0; k < qtd; k++)
    {
        unsigned char byte_lido;
        unsigned int freq;
        if (fread(&byte_lido, sizeof(unsigned char), 1, entrada) != 1 || fread(&freq, sizeof(unsigned int), 1, entrada) != 1)
        {
            printf("arquivo compactado invalido (tabela incompleta)\n");
            fclose(entrada);
            return;
        }
        tabela[byte_lido] = freq;
    }

    // reconstroi a mesma arvore que foi usada na compactacao
    No *raiz = construir_arvore(tabela);
    if (raiz == NULL)
    {
        printf("erro ao reconstruir a arvore\n");
        fclose(entrada);
        return;
    }

    FILE *saida = fopen(nome_saida, "wb");
    if (saida == NULL)
    {
        printf("erro ao criar o arquivo de saida\n");
        fclose(entrada);
        return;
    }


    No *aux = raiz;
    unsigned char byte;
    unsigned long long escritos = 0;

    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        //caso especial, arquivo com um unico tipo de byte (arvore = 1 folha).

        unsigned char unico = *(unsigned char *)raiz->dado;
        while (escritos < total_bytes)
        {
            fputc(unico, saida);
            escritos++;
        }
    }
    else
    {
        // para quando atingir total_bytes (ignora os bits de preenchimento)
        while (escritos < total_bytes && fread(&byte, sizeof(unsigned char), 1, entrada))
        {
            for (int i = 7; i >= 0 && escritos < total_bytes; i--)
            {
                if (eh_bit_um(byte, i))
                    aux = aux->dir;
                else
                    aux = aux->esq; // MUDANÇA: era aux->dir (bit 0 tem que ir pra esquerda)

                if (aux->esq == NULL && aux->dir == NULL)
                {
                    fputc(*(unsigned char *)aux->dado, saida);
                    escritos++;
                    aux = raiz;
                }
            }
        }
    }

    if (escritos < total_bytes)
        printf("aviso: o arquivo compactado terminou antes do esperado (corrompido?)\n");
    else
        printf("Arquivo descompactado com sucesso: %s\n", nome_saida);

    fclose(entrada);
    fclose(saida);
}


int main()
{
    unsigned int tabela_frequencia[TAM];
    char nome_arquivo[256];
    char nome_saida[256];
    int opcao;

    Lista lista;
    No *arvore;
    int colunas;
    char **dicionario;
    char *codificado, *decodificado;

    printf("1 - Compactar um arquivo\n2 - Descompactar um arquivo (.gh)\nEscolha: ");
    if (scanf("%d", &opcao) != 1)
        return 1;

    if (opcao == 2)
    {
        printf("Digite o nome do arquivo compactado: ");
        scanf("%s", nome_arquivo);
        printf("Digite o nome do arquivo de saida (com a extensao original, ex: foto.png): ");
        scanf("%s", nome_saida);

        descompactar(nome_arquivo, nome_saida);
        return 0;
    }
    else if (opcao != 1)
    {
        printf("Opcao invalida.\n");
        return 1;
    }

    printf("Digite o nome do arquivo que vai ser compactado: ");
    scanf("%s", nome_arquivo); 
    printf("Digite o nome do arquivo compactado (ex: compactado.gh): ");
    scanf("%s", nome_saida);

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

    //imprime_tab_freq(tabela_frequencia);

    //tamanho original do arquivo em bytes
    unsigned long long total_bytes = 0;
    for (int i = 0; i < TAM; i++)
        total_bytes += tabela_frequencia[i];


    if (total_bytes == 0)
    {
        printf("O arquivo esta vazio, nada a compactar.\n");
        return 1;
    }



    // lista encadeada + arvore de huffman
    arvore = construir_arvore(tabela_frequencia);
    if (arvore == NULL)
    {
        printf("Erro ao montar a arvore.\n");
        return 1;
    }

    //imprimir_arvore(arvore, 0);

    //montar dicionario
    colunas = altura_arvore(arvore) + 1;
    dicionario = alocar_dicionario(colunas);
    gerar_dicionario(dicionario, arvore, "", colunas);
    //imprimir_dicionario(dicionario);

    //codificar
    arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL)
    {
        printf("Erro ao reabrir o arquivo.\n");
        return 1;
    }
    codificado = codificar(dicionario, arquivo);
    fclose(arquivo);
    //printf("\n -------texto codificado:------\n%s\n", codificado);

    if (codificado == NULL)
    {
        printf("Erro ao alocar memoria em codificar.\n");
        return 1;
    }

    //compactar

    compactar((unsigned char *)codificado, tabela_frequencia, total_bytes, nome_saida);
    printf("Arquivo compactado gerado: %s\n", nome_saida);

    free(codificado);


    return 0;
}
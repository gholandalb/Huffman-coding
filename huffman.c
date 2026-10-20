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
    int byte; // para caber o EOF (257)
    while((byte = fgetc(arquivo)) != EOF)
    {
        tab[(unsigned char)byte]++; //devolve para o valor de 0 a 255, funcional para o dicionario
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


//compactar

//diz se um byte de folha precisa de '\' antes dele no cabecalho.
// Sao os dois caracteres que teriam significado especial na arvore salva: '*' e '\'.
int precisa_escape(unsigned char byte)
{
    return byte == '*' || byte == '\\';
}

// conta quantos bytes a arvore vai ocupar quando salva em pre-ordem.
// Folha = 1 byte (2 se precisar de escape). No interno = 1 byte ('*') + filhos.
// Esse valor vai nos 13 bits do cabecalho, entao precisa ser calculado ANTES de gravar a arvore.
unsigned short tamanho_arvore(No *raiz)
{
    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        if (precisa_escape(*(unsigned char *)raiz->dado))
            return 2;
        return 1;
    }
    return 1 + tamanho_arvore(raiz->esq) + tamanho_arvore(raiz->dir);
}


// grava a arvore em pre-ordem (raiz, esquerda, direita).
// No interno -> '*'.  Folha -> o proprio byte, com '\' na frente se for '*' ou '\'.
void escrever_arvore(No *raiz, FILE *arquivo)
{
    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        unsigned char byte_da_folha = *(unsigned char *)raiz->dado;
        if (precisa_escape(byte_da_folha))
            fputc('\\', arquivo);
        fputc(byte_da_folha, arquivo);
    }
    else
    {
        fputc('*', arquivo);
        escrever_arvore(raiz->esq, arquivo);
        escrever_arvore(raiz->dir, arquivo);
    }
}



// MUDANÇA: a assinatura mudou. Antes recebia (str, tab, total_bytes, nome_saida).
// Agora recebe a raiz da arvore no lugar da tabela e do total_bytes, pois o cabecalho
// novo guarda a arvore (e nao mais as frequencias), e o fim dos dados eh dado pelo "lixo".
void compactar(char nome_entrada[], char **dicionario, unsigned int tab[], No *raiz, char nome_saida[])
{
    FILE *entrada = fopen(nome_entrada, "rb");
    FILE *saida = fopen(nome_saida, "wb");
    if (entrada == NULL || saida == NULL)
    {
        printf("erro ao abrir arquivo em compactar\n");
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        return;
    }

    // total de bits = soma de (frequencia * tamanho do codigo) de cada byte
    unsigned long long qtd_bits = 0;
    for (int k = 0; k < TAM; k++)
        qtd_bits += (unsigned long long)tab[k] * strlen(dicionario[k]);

    unsigned short lixo = (8 - (qtd_bits % 8)) % 8;
    unsigned short cabecalho = (lixo << 13) | tamanho_arvore(raiz);
    fputc(cabecalho >> 8, saida);
    fputc(cabecalho & 0xFF, saida);
    escrever_arvore(raiz, saida);

    int c, j = 7;
    unsigned char byte = 0;
    while ((c = fgetc(entrada)) != EOF)
    {
        for (char *p = dicionario[(unsigned char)c]; *p; p++)
        {
            if (*p == '1')
                byte |= 1 << j;
            if (--j < 0)
            {
                fputc(byte, saida);
                byte = 0;
                j = 7;
            }
        }
    }
    if (j != 7)
        fputc(byte, saida);

    fclose(entrada);
    fclose(saida);
}


// descompactar

unsigned int eh_bit_um(unsigned char byte, int i)
{
    unsigned char mascara = (1 << i);
    return mascara & byte;
}


// cria um no com o byte guardado em void *dado (mesmo padrao do resto do codigo)
No *criar_no(unsigned char byte)
{
    No *novo = malloc(sizeof(No));
    unsigned char *byte_alocado = malloc(sizeof(unsigned char));
    if (novo == NULL || byte_alocado == NULL)
    {
        free(novo);
        free(byte_alocado);
        return NULL;
    }
    *byte_alocado = byte;
    novo->dado = byte_alocado;
    novo->frequencia = 0; // nao eh necessaria na descompactacao
    novo->esq = novo->dir = novo->prox = NULL;
    return novo;
}


//  reconstroi a arvore lendo a pre-ordem gravada no arquivo.
// '*'  -> no interno: le recursivamente o filho esquerdo e depois o direito.
// '\'  -> escape: o proximo byte eh uma folha, mesmo que seja '*' ou '\'.
// outro -> folha com aquele byte.
No *ler_arvore(FILE *entrada)
{
    int c = fgetc(entrada);
    if (c == EOF)
        return NULL;

    if (c == '*')
    {
        No *no = criar_no('+');
        if (no == NULL)
            return NULL;
        no->esq = ler_arvore(entrada);
        no->dir = ler_arvore(entrada);
        if (no->esq == NULL || no->dir == NULL)
            return NULL; // arvore corrompida/incompleta
        return no;
    }

    if (c == '\\')
    {
        c = fgetc(entrada); // byte escapado
        if (c == EOF)
            return NULL;
    }
    return criar_no((unsigned char)c);
}



//a funcao faz tudo sozinha: le o cabecalho, reconstroi a arvore e grava o resultado num arquivo. Assim funciona com qualquer .huff, mesmo de outra execucao.

void descompactar(char nome_entrada[], char nome_saida[])
{
    FILE *entrada = fopen(nome_entrada, "rb");
    if (entrada == NULL)
    {
        printf("erro ao abrir arquivo em descompactar\n");
        return;
    }

    // leitura do cabecalho novo (2 bytes). Removidos: total_bytes, qtd, tabela e o loop de leitura dos pares.
    int b1 = fgetc(entrada);
    int b2 = fgetc(entrada);
    if (b1 == EOF || b2 == EOF)
    {
        printf("arquivo compactado invalido (cabecalho corrompido)\n");
        fclose(entrada);
        return;
    }
    unsigned short cabecalho = (b1 << 8) | b2;
    unsigned short lixo = cabecalho >> 13;          // 3 bits de cima
    unsigned short tam_arvore = cabecalho & 0x1FFF; // 13 bits de baixo

    // a arvore agora eh lida direto do arquivo, nao mais reconstruida a partir de frequencias
    No *raiz = ler_arvore(entrada);
    if (raiz == NULL || (unsigned short)(ftell(entrada) - 2) != tam_arvore)
    {
        printf("erro ao reconstruir a arvore\n");
        fclose(entrada);
        return;
    }

    // como nao existe mais total_bytes, descobrimos onde os dados terminam
    // pelo tamanho do arquivo. Total de bits validos = bytes de dados * 8 - lixo.
    long inicio_dados = ftell(entrada);
    fseek(entrada, 0, SEEK_END);
    long fim_dados = ftell(entrada);
    fseek(entrada, inicio_dados, SEEK_SET);

    if (fim_dados <= inicio_dados)
    {
        printf("arquivo compactado invalido (sem dados)\n");
        fclose(entrada);
        return;
    }
    unsigned long long total_bits = (unsigned long long)(fim_dados - inicio_dados) * 8 - lixo;

    FILE *saida = fopen(nome_saida, "wb");
    if (saida == NULL)
    {
        printf("erro ao criar o arquivo de saida\n");
        fclose(entrada);
        return;
    }


    No *aux = raiz;
    unsigned char byte;
    unsigned long long bits_lidos = 0; //antes era "escritos" (contava bytes escritos); agora conta bits lidos

    if (raiz->esq == NULL && raiz->dir == NULL)
    {
        //caso especial, arquivo com um unico tipo de byte (arvore = 1 folha).
        //no compactar esse byte vira o codigo "0" (1 bit), entao ha 1 bit valido por byte original.

        unsigned char unico = *(unsigned char *)raiz->dado;
        while (bits_lidos < total_bits)
        {
            fputc(unico, saida);
            bits_lidos++;
        }
    }
    else
    {
        // para quando consumir todos os bits validos (ignora os bits de lixo do ultimo byte)
        while (bits_lidos < total_bits && fread(&byte, sizeof(unsigned char), 1, entrada))
        {
            for (int i = 7; i >= 0 && bits_lidos < total_bits; i--)
            {
                if (eh_bit_um(byte, i))
                    aux = aux->dir;
                else
                    aux = aux->esq;

                bits_lidos++; //conta cada bit processado

                if (aux->esq == NULL && aux->dir == NULL)
                {
                    fputc(*(unsigned char *)aux->dado, saida);
                    aux = raiz;
                }
            }
        }
    }

    if (bits_lidos < total_bits)
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

    printf("1 - Compactar um arquivo\n2 - Descompactar um arquivo (.huff)\nEscolha: ");
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
    
    printf("Digite o nome do arquivo compactado (ex: compactado.huff): ");
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
        printf("Arquivo vazio ou erro ao montar a arvore.\n");
        return 1;
    }

    //montar dicionario
    colunas = altura_arvore(arvore) + 1;
    dicionario = alocar_dicionario(colunas);
    gerar_dicionario(dicionario, arvore, "", colunas);

    //compactar
    compactar(nome_arquivo, dicionario, tabela_frequencia, arvore, nome_saida);
    printf("Arquivo compactado gerado: %s\n", nome_saida);

    return 0;
}
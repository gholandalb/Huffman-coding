#include <stdio.h>
#include <stdlib.h>

#define TAM 256

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
    for (int i = 0; i < TAM; i++)
    {
        if (tab[i] != 0)
        printf("\t%d = %d = %c\n", i, tab[i], i);
    }

}

int main()
{
    unsigned int tabela_frequencia[TAM];
    char nome_arquivo[256];

    printf("Digite o nome do arquivo: ");
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
    imprime_tab_freq(tabela_frequencia);

}
#include <stdio.h>
#include <stdlib.h>

#define TAM 256

void inicializa_tabela_com_0(unsigned int tab[])
{
    for (int i = 0; i < TAM; i++)
        tab[i] = 0;
}

void preenche_tab_frequencia(unsigned char texto[], unsigned int tab[])
{
    int i = 0;
    while(texto[i] != '\0')
    {
        tab[texto[i]]++;
        i++;
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
    unsigned char texto[] = "Vamos aprender a programar";
    unsigned int tabela_frequencia[TAM];

    // tabela de frequencia

    inicializa_tabela_com_0(tabela_frequencia);
    preenche_tab_frequencia(texto, tabela_frequencia);
    imprime_tab_freq(tabela_frequencia);

}
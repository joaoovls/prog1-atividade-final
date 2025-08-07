#include <stdio.h>
#include <stdlib.h> //usar para a função de randomizar número
#include <time.h>   //usar para a função de randomizar número
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h> //funções de sleep e system clear na versão windows
#include <conio.h>   //ler a entrada do teclado para sistema windows
#else
#include <unistd.h>  //função usleep e system clear
#include <ncurses.h> //ler a entrada do teclado para sistemas LINUX
#endif

// definições para o tamanho da rua e para o máximo de inimigos na tela
#define LARGURA_RUA 75
#define ALTURA_RUA 10
#define MAX_CARRO_INIMIGOS 20

// posições dos carros
typedef struct
{
    int x;
    int y;
} Carro;

// variáveis para o sistema de dia e o carro
Carro carro_jogador;
Carro carro_inimigo[MAX_CARRO_INIMIGOS];
int dia = 1;
int carros_ultrapassados = 0;
int meta_ultrapassagem = 30;
int velocidade_carro = 100;
bool gameOver = false;
bool começarJogo = false;

// funções da biblioteca ncurses.h

// funções
void entradaTeclado(char key)
{
#ifdef _WIN32

#else
    key = getch();
    switch (key)
    {
    case 'a':
    case 'A':
        // movimentação para a esquerda <--
    case 'd':
    case 'D':
        // movimentação para a direita <--
    }
#endif
}

void systemClear()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void posicaoJogador()
{
    carro_jogador.x = LARGURA_RUA / 2;
    carro_jogador.y = ALTURA_RUA - 2;
}

//OBS: tem q ajeitar essa porra
void desenharTela()
{
    posicaoJogador();
    systemClear();
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        printf("|");
        for (int j = 0; j < LARGURA_RUA; j++)
        {
            char pixel = '*';
            if (j == carro_jogador.x && i == carro_jogador.y)
            {
                pixel = 'A';
            }
            printf("%c", pixel);
        }
        printf("|\n");
    }
}

int main()
{
    initscr();
    // while(!gameOver) {
    posicaoJogador();
    desenharTela();
    //}
}
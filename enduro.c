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
#define LARGURA_RUA 25
#define ALTURA_RUA 15
#define MAX_CARRO_INIMIGOS 20
#define CHAR_JOGADOR 'A'
#define CHAR_INIMIGO 'O'
#define CHAR_VAZIO ' '

// posições dos carros
typedef struct
{
    int x;
    int y;
    bool active;
} Carro;

// variáveis para o sistema de dia e o carro
Carro carro_jogador;
Carro carro_inimigo[MAX_CARRO_INIMIGOS];
int dia = 1;
int carros_ultrapassados = 0;
int meta_ultrapassagem = 30;
bool gameOver = false;
bool começarJogo = false;

// funções da biblioteca ncurses.h

// funções
void systemClear()
{
#ifdef _WIN32
    system("cls");
#else
    clear();
#endif
}

//movimentação dos carros inimigos
void atualizarInimigos()
{
    if (rand() % 100 < 15)
    {
        for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
        {
            if (carro_inimigo[i].active == false)
            {
                carro_inimigo[i].active = true;
                carro_inimigo[i].y = 0;
                carro_inimigo[i].x = rand() % LARGURA_RUA + 1;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        if (carro_inimigo[i].active == true)
        {
            carro_inimigo[i].y++;
            if (carro_inimigo[i].y > ALTURA_RUA + 1)
            {
                carro_inimigo[i].active = false;
            }
        }
    }
}

void posicaoJogador()
{
    carro_jogador.x = LARGURA_RUA / 2;
    carro_jogador.y = ALTURA_RUA - 2;
}

void desenharTela(int inicio_y, int inicio_x)
{
#ifdef _WIN32

#else
    // printa os limites da rua
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        mvaddch(inicio_y + i, inicio_x, '|');
        mvaddch(inicio_y + i, inicio_x + LARGURA_RUA + 1, '|');
    }

    // printa os espaços vazios, o carro do jogador e os carros inimigos
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        for (int j = 0; j < LARGURA_RUA; j++)
        {
            char caractere_carro = CHAR_VAZIO;

            for (int k = 0; k < MAX_CARRO_INIMIGOS; k++)
            {
                if (carro_inimigo[k].active == true && j == carro_inimigo[k].x && i == carro_inimigo[k].y)
                {
                    caractere_carro = CHAR_INIMIGO;
                    break;
                }
            }

            if (carro_jogador.x == j && carro_jogador.y == i)
            {
                caractere_carro = CHAR_JOGADOR;
            }

            mvaddch(inicio_y + i, inicio_x + 1 + j, caractere_carro);
        }

        mvprintw(ALTURA_RUA - 1, inicio_x + 1, "Dias: %d", dia);
    }
#endif
}

int main()
{
    srand(time(NULL));
    // inicialização do ncurses, não aparecer a tecla no terminal, desativar o cursor do terminal e apertar a tecla sem precisar digitar ENTER
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    // tamanho real do terminal
    int terminal_x, terminal_y;
    getmaxyx(stdscr, terminal_y, terminal_x);

    // localização da rua em relação ao terminal
    int inicio_y = (terminal_y - ALTURA_RUA) / 2;
    int inicio_x = (terminal_x - LARGURA_RUA) / 2;

    posicaoJogador();

    for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        carro_inimigo[i].active = false;
    }

    // controles do jogador e desenho na tela
    char key;
    while ((key = getch()) != 'q')
    {

        if (gameOver == true) {
            break;
        }

#ifdef _WIN32

#else
        switch (key)
        {
        case 'a':
        case 'A':
            if (carro_jogador.x > 0)
            {
                carro_jogador.x--;
            }
            break;

        case 'd':
        case 'D':
            if (carro_jogador.x < LARGURA_RUA - 1)
            {
                carro_jogador.x++;
            }
            break;
        }

        atualizarInimigos();

        //detecta se algum carro inimigo colidiu com o jogador
        for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        if (carro_inimigo[i].y == carro_jogador.y && carro_inimigo[i].x == carro_jogador.x)
        {
            gameOver = true;
            break;
        }
    }

        // atualização da tela
        systemClear();
        desenharTela(inicio_y, inicio_x);
        refresh();

        usleep(50000); //controlar os quadros por segundo
#endif
    }

    // encerramento do programa
    endwin();
    return 0;
}
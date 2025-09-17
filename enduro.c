#include <stdio.h>
#include <stdlib.h> //usar para a função de randomizar número
#include <time.h>   //usar para a função de randomizar número
#include <stdbool.h>
#include <unistd.h>  //função usleep e system clear
#include <ncurses.h> //ler a entrada do teclado para sistemas LINUX

// definições para o tamanho da rua e para o máximo de inimigos na tela
#define LARGURA_RUA 25
#define ALTURA_RUA 15
#define MAX_CARRO_INIMIGOS 20
#define CHAR_JOGADOR 'A'
#define CHAR_INIMIGO 'O'
#define CHAR_VAZIO ' '
#define CHAR_SETA '>'

// variáveis dos carros
typedef struct
{
    int x;
    int y;
    bool active;
} Carro;

// variáveis relacionadas aos carros
Carro carro_jogador;
Carro carro_inimigo[MAX_CARRO_INIMIGOS];
Carro seta_menu;
int dia = 1;
int carros_ultrapassados = 0;
int meta_ultrapassagem = 60;
int usleep_velocidade = 60000; // pra usar no usleep

bool noite = false;

// para a pontuação
int contador = 0;
int score = 0;

// chance de spawn dos inimigos
int chance_spawn = 15;

// finalização
bool gameOver = false;

// menu inicial do jogo
void menuInicial(int terminal_y, int terminal_x)
{
    // posição inicial da seta
    seta_menu.y = terminal_y - 10;
    seta_menu.x = terminal_x / 2 - 3;

    char key;

    // laço do menu
    while (true)
    {
        clear();

        attron(A_BOLD);
        mvprintw(terminal_y - 11, terminal_x / 2, "ENDURO");
        attroff(A_BOLD);
        mvprintw(terminal_y - 10, terminal_x / 2, "JOGAR");
        mvprintw(terminal_y - 8, terminal_x / 2, "SAIR");
        mvaddch(seta_menu.y, seta_menu.x, CHAR_SETA);
        refresh();
        usleep(60000);

        key = getch();
        switch (key)
        {
        case 'w':
        case 'W':
            if (seta_menu.y > terminal_y - 10)
            {
                seta_menu.y -= 2;
                break;
            }

        case 's':
        case 'S':
        {
            if (seta_menu.y < terminal_y - 8)
            {
                seta_menu.y += 2;
                break;
            }
        }
        }

        if (key == '\n')
        {
            if (seta_menu.y == terminal_y - 10)
            {
                break;
            }

            if (seta_menu.y == terminal_y - 8)
            {
                endwin();
                exit(0);
            }
        }
    }
}

void desenharTela(int terminal_y, int terminal_x, int inicio_y, int inicio_x, int HUD_x)
{

    // mudança de cor do cenário
    int cor_ceu = noite ? 2 : 1;

    attron(COLOR_PAIR(cor_ceu));
    for (int y = 0; y < terminal_y; y++)
    {
        for (int x = 0; x < terminal_x; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    attroff(COLOR_PAIR(cor_ceu));

    // printa os limites da rua
    attron(COLOR_PAIR(cor_ceu));
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        mvaddch(inicio_y + i, inicio_x, '|');
        mvaddch(inicio_y + i, inicio_x + LARGURA_RUA + 1, '|');
    }
    attroff(COLOR_PAIR(cor_ceu));

    int cor_estrada = noite ? 4 : 3;
    attron(COLOR_PAIR(cor_estrada));
    if (noite)
        attron(A_BOLD);

    // printa os espaços vazios, o carro do jogador e os carros inimigos
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        for (int j = 0; j < LARGURA_RUA; j++)
        {
            attron(A_BOLD);
            mvaddch(ALTURA_RUA, inicio_x + j + 1, '=');
            attroff(A_BOLD);
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
    }

    if (noite)
        attroff(A_BOLD);
    attroff(COLOR_PAIR(cor_estrada));

    attron(COLOR_PAIR(cor_ceu));
    mvprintw(ALTURA_RUA / 2 + 2, HUD_x - 2, "Dias: %d", dia);
    mvprintw(ALTURA_RUA / 2 + 3, HUD_x - 2, "Ultrapassagens restantes: %d", meta_ultrapassagem);
    mvprintw(ALTURA_RUA / 2 + 4, HUD_x - 2, "Pontuação: %d", score);
    attroff(COLOR_PAIR(cor_ceu));
}

// movimentação dos carros inimigos
void logicaInimigos()
{
    if (rand() % 100 < chance_spawn)
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

        if (carro_inimigo[i].active == true && carro_inimigo[i].y > ALTURA_RUA)
        {
            meta_ultrapassagem--;
            carros_ultrapassados++;
        }

        if (carro_inimigo[i].y == carro_jogador.y && carro_inimigo[i].x == carro_jogador.x)
        {
            gameOver = true;
            break;
        }
    }
}

void posicaoJogador()
{
    carro_jogador.x = LARGURA_RUA / 2;
    carro_jogador.y = ALTURA_RUA - 2;
}

// lógica de pontuação e sistema do jogo
void pontuacao()
{
    contador++;
    if (contador >= 10)
    {
        score++;
        contador = 0;
    }

    if (meta_ultrapassagem <= 0)
    {
        dia++;
        chance_spawn += 5;

        if (chance_spawn > 80)
        {
            chance_spawn = 80;
        }

        meta_ultrapassagem = 45 + (dia * 15);
    }

    if (dia % 2 == 0)
    {
        noite = true;
    }
    else
    {
        noite = false;
    }
}

int main()
{
    // mudanças no terminal
    srand(time(NULL));
    initscr();

    // mudança de cor do terminal
    start_color();
    // Em main(), depois de start_color()
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    // tamanho real do terminal
    int terminal_x, terminal_y;
    getmaxyx(stdscr, terminal_y, terminal_x);

    // localização da rua em relação ao terminal
    int inicio_y = (terminal_y - ALTURA_RUA) / 2;
    int inicio_x = (terminal_x - LARGURA_RUA) / 2;

    int HUD_x = (terminal_x / 3 - 9);

    for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        carro_inimigo[i].active = false;
    }

    menuInicial(terminal_y, terminal_x);

    posicaoJogador();

    // controles do jogador e desenho na tela
    char key;
    while ((key = getch()) != 'q')
    {
        if (gameOver == true)
        {
            while (true)
            {
                clear();
                attron(A_BOLD);
                mvprintw(terminal_y - 11, terminal_x / 2, "Você perdeu!");
                attroff(A_BOLD);
                mvprintw(terminal_y - 9, terminal_x / 2 - 17, "Você sobreviveu por %d dias e ultrapassou %d carros", dia, carros_ultrapassados);
                usleep(60000);
                refresh();
                getch();
            }
        }

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

        logicaInimigos();

        // atualização da tela
        clear();
        desenharTela(terminal_y, terminal_x,inicio_y, inicio_x, HUD_x);
        pontuacao();
        refresh();
        usleep(usleep_velocidade); // controlar os quadros por segundo

        // encerramento do programa
        endwin();
    }
    return 0;
}
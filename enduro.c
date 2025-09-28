#include <stdlib.h> //usar para a função de randomizar número
#include <time.h>   //usar para a função de randomizar número
#include <stdbool.h>
#include <unistd.h>  //função usleep e system clear
#include <ncurses.h> //ler a entrada do teclado para sistemas LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// definições para o tamanho da rua e para o máximo de inimigos na tela
#define LARGURA_RUA 50
#define ALTURA_RUA 35
#define MAX_CARRO_INIMIGOS 8
#define STR_CARRO1 ".--."
#define STR_CARRO2 "|____|"
#define CHAR_VAZIO ' '
#define CHAR_SETA '>'
#define MAX_NUVENS 10

typedef struct
{
    float x;
    int y;
    int largura;
    float velocidade;
    bool active;
} Nuvem;

Nuvem nuvens[MAX_NUVENS];

// variáveis dos carros
typedef struct
{
    int x;
    int y;
    bool active;
    int largura;
    int altura;
} Carro;

// variáveis relacionadas aos carros
Carro carro_jogador;
Carro carro_inimigo[MAX_CARRO_INIMIGOS];
Carro seta_menu;
int dia = 1;
int carros_ultrapassados = 0;
int meta_ultrapassagem = 60;
int usleep_velocidade = 60000;
bool noite = false;
int contador = 0;
int score = 0;
int chance_spawn = 15;
bool gameOver = true;
int iframes = 0;
int contador_animacao = 0;
// parte de cima = ".--.", "|____|, 6;2"

void menuInicial(int terminal_y, int terminal_x)
{
    // posição inicial da seta
    seta_menu.y = terminal_y - 29;
    seta_menu.x = terminal_x / 2 - 3;

    char key;

    // laço do menu
    while (true)
    {
        clear();

        attron(A_BOLD);
        mvprintw(terminal_y - 33, terminal_x / 2, "ENDURO");
        attroff(A_BOLD);
        mvprintw(terminal_y - 29, terminal_x / 2, "JOGAR");
        mvprintw(terminal_y - 27, terminal_x / 2, "SAIR");
        mvaddch(seta_menu.y, seta_menu.x, CHAR_SETA);
        refresh();
        usleep(60000);

        key = getch();
        switch (key)
        {
        case 'w':
        case 'W':
            if (seta_menu.y > terminal_y - 29)
            {
                seta_menu.y -= 2;
                break;
            }
        case 's':
        case 'S':
        {
            if (seta_menu.y < terminal_y - 27)
            {
                seta_menu.y += 2;
                break;
            }
        }
        }

        if (key == '\n')
        {
            if (seta_menu.y == terminal_y - 29)
            {
                break;
            }

            if (seta_menu.y == terminal_y - 27)
            {
                endwin();
                exit(0);
            }
        }
    }
}

void desenharNuvem(int terminal_x)
{
    if (!noite)
    {
        {
            for (int i = 0; i < MAX_NUVENS; i++)
            {
                if (!nuvens[i].active)
                {
                    nuvens[i].active = true;
                    nuvens[i].x = terminal_x;
                    nuvens[i].y = rand() % 5;
                    nuvens[i].largura = 5 + rand() % 10;
                    nuvens[i].velocidade = 0.2f + (float)(rand() % 5) / 3.0f;
                    break;
                }
            }
        }
    }
}

void atualizarNuvem()
{
    for (int i = 0; i < MAX_NUVENS; i++)
    {
        if (nuvens[i].active)
        {
            nuvens[i].x -= nuvens[i].velocidade;
            if (nuvens[i].x + nuvens[i].largura < 0)
            {
                nuvens[i].active = false;
            }
        }
    }
}

void desenharTela(int terminal_y, int terminal_x, int inicio_y, int inicio_x, int HUD_x)
{

    // mudança de cor do cenário
    int cor_ceu = noite ? 2 : 1;

    attron(COLOR_PAIR(5));
    for (int y = 0; y < terminal_y; y++)
    {
        for (int x = 0; x < terminal_x; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(cor_ceu));
    for (int y = 0; y < ALTURA_RUA / 2 - 7; y++)
    {
        for (int x = 0; x < terminal_x; x++)
        {
            mvaddch(y, x, ' ');
        }
    }
    attroff(COLOR_PAIR(cor_ceu));

    attron(COLOR_PAIR(3));
    for (int i = 0; i < MAX_NUVENS; i++)
    {
        if (nuvens[i].active)
        {
            for (int j = 0; j < nuvens[i].largura; j++)
            {
                int posX = (int)nuvens[i].x + j;
                if (posX >= 0 && posX < terminal_x)
                {
                    mvaddch(nuvens[i].y, posX, ' ');
                }
            }
        }
    }
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        for (int j = 0; j < LARGURA_RUA; j++)
        {
            mvaddch(inicio_y + i, inicio_x + 1 + j, ' ');
        }
    }
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(3));
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        if ((i - contador_animacao % 8 + 8) % 8 < 4)
        {
            int faixa1_x = inicio_x + LARGURA_RUA / 3;
            int faixa2_x = inicio_x + (LARGURA_RUA * 2) / 3;
            mvaddch(inicio_y + i, faixa1_x, '|');
            mvaddch(inicio_y + i, faixa2_x, '|');
        }
    }
    attroff(COLOR_PAIR(3));

    // printa os limites da rua
    attron(COLOR_PAIR(3));
    for (int i = 0; i < ALTURA_RUA; i++)
    {
        mvaddch(inicio_y + i, inicio_x, '|');
        mvaddch(inicio_y + i, inicio_x + LARGURA_RUA + 1, '|');
    }
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    for (int k = 0; k < MAX_CARRO_INIMIGOS; k++)
    {
        if (carro_inimigo[k].active)
        {
            mvprintw(inicio_y + carro_inimigo[k].y, inicio_x + 1 + carro_inimigo[k].x, STR_CARRO1);
            mvprintw(inicio_y + carro_inimigo[k].y + 1, inicio_x + carro_inimigo[k].x, STR_CARRO2);
        }
    }
    mvprintw(inicio_y + carro_jogador.y, inicio_x + 1 + carro_jogador.x, STR_CARRO1);
    mvprintw(inicio_y + carro_jogador.y + 1, inicio_x + carro_jogador.x, STR_CARRO2);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(5));
    mvprintw(ALTURA_RUA + 12, HUD_x + 30, "Dias: %d", dia);
    mvprintw(ALTURA_RUA + 12, HUD_x + 45, "Ultrapassagens restantes: %d", meta_ultrapassagem);
    mvprintw(ALTURA_RUA + 15, HUD_x + 27, "Pontuação: %d", score);
    mvprintw(ALTURA_RUA + 15, HUD_x + 51, "Aperte Q para sair.");
    attroff(COLOR_PAIR(5));
}

// movimentação dos carros inimigos
void logicaInimigos()
{
    if (rand() % 100 < chance_spawn)
    {
        bool areaLivreEntreCarros = true;
        int distanciaMinima = 6;

        for (int k = 0; k < MAX_CARRO_INIMIGOS; k++)
        {
            if (carro_inimigo[k].active && carro_inimigo[k].y < distanciaMinima)
            {
                areaLivreEntreCarros = false;
                break;
            }
        }

        if (areaLivreEntreCarros)
        {
            for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
            {
                if (carro_inimigo[i].active == false)
                {
                    carro_inimigo[i].active = true;
                    carro_inimigo[i].y = 0;

                    carro_inimigo[i].x = rand() % (LARGURA_RUA - 6);
                    break;
                }
            }
        }
    }

    for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        if (carro_inimigo[i].active == true)
        {
            carro_inimigo[i].y++;

            if (carro_inimigo[i].y > ALTURA_RUA - 2)
            {
                carro_inimigo[i].active = false;
            }

            if (carro_inimigo[i].active == true && carro_inimigo[i].y > ALTURA_RUA - 3)
            {
                meta_ultrapassagem--;
                carros_ultrapassados++;
            }

            bool colisao_x = (carro_jogador.x < carro_inimigo[i].x + 6) && (carro_jogador.x + 6 > carro_inimigo[i].x);

            bool colisao_y = (carro_jogador.y < carro_inimigo[i].y + 2) && (carro_jogador.y + 2 > carro_inimigo[i].y);

            if (iframes <= 0)
            {
                if (colisao_x && colisao_y)
                {
                    gameOver = true;
                }
            }
        }
    }
}

void posicaoJogador()
{
    carro_jogador.x = LARGURA_RUA / 2;
    carro_jogador.y = ALTURA_RUA - 3;
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

        usleep_velocidade -= 3000;

        if (usleep_velocidade <= 25000)
        {
            usleep_velocidade = 25000;
        }
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

void reiniciarJogo()
{
    posicaoJogador();

    dia = 1;
    carros_ultrapassados = 0;
    meta_ultrapassagem = 60;
    usleep_velocidade = 60000;
    noite = false;
    contador = 0;
    score = 0;
    chance_spawn = 15;
    gameOver = false;
    iframes = 5;

    for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        carro_inimigo[i].active = false;
    }

    for (int i = 0; i < MAX_NUVENS; i++)
    {
        nuvens[i].active = false;
    }

    flushinp();
}

int main()
{

    for (int i = 0; i < MAX_CARRO_INIMIGOS; i++)
    {
        carro_inimigo[i].active = false;
    }

    for (int i = 0; i < MAX_NUVENS; i++)
    {
        nuvens[i].active = false;
    }

    // inicialização ncurses e sdl2 mixer
    srand(time(NULL));
    initscr();
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *somMotor = Mix_LoadWAV("/home/joao/Downloads/motor_enduro.wav");
    bool somInicializado = false;

    // mudança de cor do terminal
    start_color();
    init_color(8, 500, 500, 500);
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_WHITE);
    init_pair(4, COLOR_BLACK, 8);
    init_pair(5, COLOR_BLACK, COLOR_GREEN);

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

    menuInicial(terminal_y, terminal_x);

    posicaoJogador();
    // controles do jogador e desenho na tela
    char key;
    while ((key = getch()) != 'q')
    {
        if (!somInicializado)
        {
            Mix_PlayChannel(-1, somMotor, -1);
            somInicializado = true;
        }

        if (iframes > 0)
        {
            iframes--;
        }

        if (gameOver == true)
        {
            Mix_HaltChannel(-1);
            somInicializado = false;

            char x;
            while (true)
            {
                clear();
                attron(A_BOLD);
                mvprintw(terminal_y - 28, terminal_x / 2, "Você perdeu!");
                attroff(A_BOLD);
                mvprintw(terminal_y - 23, terminal_x / 2 - 17, "Você sobreviveu por %d dias e ultrapassou %d carros", dia, carros_ultrapassados);
                mvprintw(terminal_y - 17, terminal_x / 2 - 17, "Aperte Q para sair");
                mvprintw(terminal_y - 17, terminal_x / 2 + 7, "Aperte R para Reiniciar");

                usleep(60000);
                refresh();

                x = getch();
                if (x == 'r' || x == 'R')
                {
                    reiniciarJogo();
                    break;
                }
                else if (x == 'q' || x == 'Q')
                {
                    endwin();
                    exit(0);
                }
                usleep(60000);
            }
        }

        switch (key)
        {
        case 'a':
        case 'A':
            if (carro_jogador.x > 0)
            {
                carro_jogador.x -= 3;
            }
            break;

        case 'd':
        case 'D':
            if (carro_jogador.x < LARGURA_RUA - 6)
            {
                carro_jogador.x += 3;
            }
            break;
        }

        contador_animacao++;

        desenharNuvem(terminal_x);
        atualizarNuvem();

        logicaInimigos();
        clear();
        desenharTela(terminal_y, terminal_x, inicio_y, inicio_x, HUD_x);
        pontuacao();
        refresh();
        usleep(usleep_velocidade); // controlar os quadros por segundo
    }

    // encerramento do programa
    Mix_FreeChunk(somMotor);
    Mix_CloseAudio();
    SDL_Quit();

    endwin();

    return 0;
}
#include <stdio.h>
#include <stdlib.h> //usar para a função de randomizar número
#include <time.h> //usar para a função de randomizar número
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h> //funções de sleep e system clear na versão windows
#include <conio.h> //ler a entrada do teclado para sistema windows
#else
#include <unistd.h> //função usleep e system clear
#include <ncurses.h> //ler a entrada do teclado para sistemas LINUX
#endif

//definições para o tamanho da rua e para o máximo de inimigos na tela 
#define COMPRIMENTO_RUA 100
#define LARGURA_RUA 75
#define CARRO_JOGADOR  'A'
#define CARRO_INIMIGO  'O' 
#define MAX_CARRO_INIMIGOS 20

//posições dos carros
typedef struct {
    int x;
    int y;
} Carro;

//variáveis para o sistema de dia e o carro
Carro carro_jogador;
Carro carro_inimigo[MAX_CARRO_INIMIGOS];
int dia = 1;
int carros_ultrapassados = 0;
int meta_ultrapassagem = 30;
int velocidade_carro = 100;
bool gameOver = false;
bool começarJogo = false;

//funções da biblioteca ncurses.h



//funções
void entradaTeclado(char key) {
    #ifdef _WIN32

    #else
        key = getch();
        switch (key) {
            case 'a':
            case 'A':
                //movimentação para a esquerda <--
            case 'd':
            case 'D':
                //movimentação para a direita <--
        }
    #endif    
}

int main(){
    initscr();
    while(!gameOver) {
        
    }
}
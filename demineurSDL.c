//Les includes
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#undef main
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEXTURE_COUNT 13

//Définition de notre variable globale
int size = 9;

//Pré définition des diverses fonctions
int TableSize();
void PrepareGrid(char** grid);
void SDLPrintGrid(char** grid, SDL_Texture* imgTabl[TEXTURE_COUNT]);
void PlaceMine(char** grid, int playerX, int playerY);
void checkMine(char** grid, int x, int y);
void makePlay(char** grid, char** bombGrid, int positionX, int positionY);
void putFlag(char** grid, char** bombGrid, int positionX, int positionY);
short isMine(char** grid, int positionX, int positionY, short gameState);
short win(short gameState, char** grid);
//void PlayMinesweeper(char** grid, char** grido);
void CreateWindow();

typedef int bool;
#define false 0
#define true 1

//Pré définir les fonction du SDL 
SDL_Renderer* renderer;
SDL_Window* window;
bool isRunning;
bool fullscreen;
void handleEvents(int* X, int* Y, char* playerAction, int* playCount);
void init_game_loop(char** gameGrid, char** realGrid);
void gameLoop(char** gameGrid, char** realGrid, SDL_Texture* imgTabl[TEXTURE_COUNT]);

//le main 
int main() {

    SDL_Init(SDL_INIT_AUDIO);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Mix_Music* backgroundSound = Mix_LoadMUS("music/backgroundmusic.mp3");

    Mix_PlayMusic(backgroundSound, -1);

    //La taille du tableau 
    //size = TableSize();
    if (size <= 0)
        return 1;

    // Malloc pour la grid
    char** grid = (char**)malloc(sizeof(char*) * size);
    if (grid == NULL)
        return 1;

    for (int i = 0; i < size; ++i)
    {
        grid[i] = (char*)malloc(sizeof(char) * size);
        if (grid[i] == NULL)
            return 1;
    }

    //Malloc pour la grido 
    char** grido = (char**)malloc(sizeof(char*) * size);
    if (grido == NULL)
        return 1;

    for (int i = 0; i < size; ++i)
    {
        grido[i] = (char*)malloc(sizeof(char) * size);
        if (grido[i] == NULL)
            return 1;
    }

    CreateWindow();

    SDL_Surface* tmpTabl[TEXTURE_COUNT] =
    {
        IMG_Load("img/1.png"),IMG_Load("img/2.png"),IMG_Load("img/3.png"),IMG_Load("img/4.png"),IMG_Load("img/5.png"),IMG_Load("img/6.png"),IMG_Load("img/7.png"),IMG_Load("img/8.png"),IMG_Load("img/flag.png"), IMG_Load("img/bomb.png"),IMG_Load("img/bground.png"),IMG_Load("img/win.png"),IMG_Load("img/lose.png")
    };
    SDL_Texture* imgTabl[TEXTURE_COUNT];

    for (int tmp = 0; tmp < TEXTURE_COUNT;tmp++)
    {
        imgTabl[tmp] = SDL_CreateTextureFromSurface(renderer, tmpTabl[tmp]);
    }
    init_game_loop(grid, grido);
    gameLoop(grid, grido, imgTabl);
    free(grid, grido);

    Mix_FreeMusic(backgroundSound);
    Mix_CloseAudio();
}

//fonction de création de Window 
void CreateWindow() 
{
    window = SDL_CreateWindow(&window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, SDL_WINDOW_RESIZABLE);

    renderer = SDL_CreateRenderer(window, -1, 0);
}


//Evènement des clic 
void handleEvents(int *X, int *Y, char *playerAction ,int *playCount) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0)
    {
        switch (ev.type)
        {

        case SDL_MOUSEBUTTONDOWN:
            if (ev.button.button == SDL_BUTTON_LEFT)
            {
                printf("Clic gauche ! \n");
                *playerAction = 'P';
                (*playCount)++;
            }
            if (ev.button.button == SDL_BUTTON_RIGHT)
            {
                printf("DRAPEAU ! \n");
                *playerAction = 'F';
                (*playCount)++;
            }
            int clickX, clickY;
            SDL_GetMouseState(&clickY, &clickX);
            *X = (clickX - 230) / 60;
            *Y = (clickY - 230) / 60;
            printf("%d %d\n", *X, *Y);
            break;


        case SDL_QUIT:
            isRunning = false;
            return Y, X;
        default:
            return Y, X;
        }

    }
}

void init_game_loop(char** gameGrid, char** realGrid)
{
	PrepareGrid(gameGrid);
	PrepareGrid(realGrid);
	
}

//fonction pour créer la boucle de jeu 
void gameLoop(char** gameGrid, char** realGrid, SDL_Texture* imgTabl[TEXTURE_COUNT]) {
	

	int playCount = -1;
    int gameState = 0;
    double target_delta_time = 1000 / 60.0;
	float delta_time = 0;
	float fps = 1 / delta_time;
	int X = -1;
	int Y = -1;
	char playerAction = ' ';



    while (gameState == 0)
	{
        Uint32 start_frame = SDL_GetTicks();
        SDLPrintGrid(gameGrid, imgTabl);
        handleEvents(&X, &Y, &playerAction, &playCount);

        if (playCount == 0)
        {
            PlaceMine(realGrid, X, Y);
            makePlay(gameGrid, realGrid, X, Y);
            playCount = 1;
        }
        else if (playCount > 0)
        {
            if (X != -1 && Y != -1)
            {
                if (playerAction == 'F')
                {
                    putFlag(gameGrid, realGrid, X, Y);
                }
                else if (playerAction == 'P')
                {
                    makePlay(gameGrid, realGrid, X, Y);
                    gameState = isMine(gameGrid, X, Y, gameState);
                }
            }     
        }

        gameState = win(gameState, gameGrid);

		Uint32 end_frame = SDL_GetTicks();

		delta_time = (float) (end_frame - start_frame);
		if (delta_time < target_delta_time)
		{
			SDL_Delay((Uint32) (target_delta_time - delta_time));
			delta_time = target_delta_time;
		}

        X = -1;
        Y = -1;

		fps = 1000 / delta_time;
        SDL_RenderClear(renderer);
	}
	//Gagné
	SDLPrintGrid(realGrid, imgTabl);
    
    SDL_Rect ending = { 230, 230, 540, 540 };
	if (gameState == 1)
	{     

        SDL_RenderCopy(renderer, imgTabl[11], NULL, &ending);
		printf("\033[0;37mYou win ! Congratulations ! ");
	}
	//Perdu
	else if (gameState == 2)
	{
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        Mix_Music* loseSoundEffect = Mix_LoadMUS("music/deathsoundeffect.mp3");
        Mix_PlayMusic(loseSoundEffect, NULL);
       

        SDL_RenderCopy(renderer, imgTabl[12], NULL, &ending);
        printf("\033[0;37mSadly, you landed on a mine, might have more luck next time !");
        SDL_Delay(3000);
        Mix_FreeMusic(loseSoundEffect);
        Mix_CloseAudio();
	}
    SDL_RenderPresent(renderer);
    SDL_Delay(10000);
    return 0;

    SDL_RenderClear(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


//génération de La taille du tableau 
int TableSize()
{
    int Taille;
    printf("Veuillez saisir la taille du tableau :");
    scanf_s("%d", &Taille);
    return Taille;
}

void SDLPrintGrid(char** grid, SDL_Texture* imgTabl[TEXTURE_COUNT])
{
    SDL_Texture* SDL_CreateTexture(SDL_Renderer * renderer,
        Uint32		 format,
        int           access,
        int			 w,
        int			 h);

    SDL_Texture* texture = NULL;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, 200, 200);
    int x = 230;
    int y = 230;
    int nbLine = 1;
    int nbCase = 1;


    SDL_Rect full = { 0, 0, 1000, 1000 };
    //SDL_SetRenderDrawColor(renderer, 252, 231, 172, 255);
    //SDL_RenderFillRect(renderer, &full);
    SDL_RenderCopy(renderer, imgTabl[10], NULL, &full);

    for (int i = 0; i < 81; i++) 
    {
        int local_x = (x - 230) / 60;
        int local_y = (y - 230) / 60;
        SDL_Rect rect = { x, y, 60, 60 };
        if ((grid[local_y][local_x] == '.' && nbCase % 2 == 0) || (grid[local_y][local_x] == 'F' && nbCase % 2 == 0))
            SDL_SetRenderDrawColor(renderer, 70, 63, 50, 255);
        else if (grid[local_y][local_x] != '.' && nbCase % 2 == 0)
            SDL_SetRenderDrawColor(renderer, 250, 240, 230, 255);
        else if ((grid[local_y][local_x] == '.' && nbCase % 2 != 0) || (grid[local_y][local_x] == 'F' && nbCase % 2 != 0))
            SDL_SetRenderDrawColor(renderer, 48, 48, 48, 255);
        else if (grid[local_y][local_x] != '.' && nbCase % 2 != 0)
            SDL_SetRenderDrawColor(renderer, 234, 192, 150, 255);
      
        SDL_RenderFillRect(renderer, &rect);

        

        if (grid[local_y][local_x] == '1')
            SDL_RenderCopy(renderer, imgTabl[0], NULL, &rect);
        else if (grid[local_y][local_x] == '2')
            SDL_RenderCopy(renderer, imgTabl[1], NULL, &rect);
        else if (grid[local_y][local_x] == '3')
            SDL_RenderCopy(renderer, imgTabl[2], NULL, &rect);
        else if (grid[local_y][local_x] == '4')
            SDL_RenderCopy(renderer, imgTabl[3], NULL, &rect);
        else if (grid[local_y][local_x] == '5')
            SDL_RenderCopy(renderer, imgTabl[4], NULL, &rect);
        else if (grid[local_y][local_x] == '6')
            SDL_RenderCopy(renderer, imgTabl[5], NULL, &rect);
        else if (grid[local_y][local_x] == '7')
            SDL_RenderCopy(renderer, imgTabl[6], NULL, &rect);
        else if (grid[local_y][local_x] == '8')
            SDL_RenderCopy(renderer, imgTabl[7], NULL, &rect);
        else if (grid[local_y][local_x] == 'F')
            SDL_RenderCopy(renderer, imgTabl[8], NULL, &rect);
        else if (grid[local_y][local_x] == 'B')
            SDL_RenderCopy(renderer, imgTabl[9], NULL, &rect);

        x += 60;
        if (i == 8 || i == 9 * nbLine - 1)
        {
            x = 230;
            y += 60;
            nbLine++;
        }
        nbCase++;
    }

    SDL_RenderPresent(renderer);
}

//Fonction pour attribuer une valeur par défaut à chaque élément de la grille
void PrepareGrid(char** grid)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            grid[i][j] = '.';
        }
    }
}

//Fonction pour placer les mines de manière aléatoire en fonction de la taille du tableau 
void PlaceMine(char** grid,int playerX , int playerY)
{
    int MineNumber = size * size / 10;
    srand(time(NULL));

    int nbCase = size * size;
    int X;
    int Y;
    int k = 0;
    int** arr = (int**)malloc(sizeof(int*) * nbCase);
    for (int i = 0; i < nbCase; i++)
        arr[i] = i;

    for (int l = playerX + 1; l >= playerX - 1; l--)
    {
        for (int m = playerY + 1; m >= playerY - 1; m--)
        {
            if (l >= 0 && m >= 0 && l < size && m < size)
            {
                int indice = l * size + m;
                for (int n = indice; n < (size * size) - 1; n++)
                {
                    arr[n] = arr[n + 1];
                }
            }    
        }
    }
    while (MineNumber > 0)
    {
        int coord = arr[(rand() % (nbCase - k))];
        X = coord / size;
        Y = coord % size;
        grid[X][Y] = 'B';

        for (int j = coord; j < nbCase - k - 1; j++)
        {
            arr[j] = arr[j + 1];
        }
        
        MineNumber--;
        k++;
    }
    free(arr);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (grid[i][j] != 'B')
            {
                checkMine(grid, i, j);
            }
        }
    }
}

//Verifie si il y'a des mines autour de la case donnée
void checkMine(char** grid, int x, int y)
{
    int countBombs = 0;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            if (i >= 0 && i < size && j >= 0 && j < size)
            {
                if (grid[i][j] == 'B')
                    countBombs++;
            }
        }
    }
    char nbBombs = countBombs + '0';
    grid[x][y] = nbBombs;
}

//Fonction récursive pour libérer les cases bonnes aux alentours
void makePlay(char** grid, char** bombGrid, int positionX, int positionY)
{
    if (positionX < 0 || positionX >= size || positionY < 0 || positionY >= size)
    {
        return;
    }

    if (grid[positionX][positionY] != '.')
    {
        return;
    }

    if (bombGrid[positionX][positionY] == '0')
    {
        grid[positionX][positionY] = ' ';
    }

    if (bombGrid[positionX][positionY] != '0')
    {
        grid[positionX][positionY] = bombGrid[positionX][positionY];
        return;
    }

    makePlay(grid, bombGrid, positionX - 1, positionY - 1);
    makePlay(grid, bombGrid, positionX - 1, positionY);
    makePlay(grid, bombGrid, positionX - 1, positionY + 1);
    makePlay(grid, bombGrid, positionX, positionY - 1);
    makePlay(grid, bombGrid, positionX, positionY + 1);
    makePlay(grid, bombGrid, positionX + 1, positionY - 1);
    makePlay(grid, bombGrid, positionX + 1, positionY);
    makePlay(grid, bombGrid, positionX + 1, positionY + 1);
}

//Fonction pour mettre un drapeau sur la case donnée
void putFlag(char** grid, char** bombGrid, int positionX, int positionY)
{
    if (grid[positionX][positionY] != '.' && grid[positionX][positionY] != 'F')
    {
        return;
    }
    else if (grid[positionX][positionY] == '.')
    {
        grid[positionX][positionY] = 'F';
    }
    else
    {
        grid[positionX][positionY] = '.';
    }
}



//Fonction pour vérifier si la case donnée est une mine
short isMine(char** grid, int positionX, int positionY, short gameState)
{
    if (grid[positionX][positionY] == 'B')
    {
        gameState = 2;
        return gameState;
    }
    else
    {
        return 0;
    }
}

//Fonction pour vérifier si l'utilisateur a gagné ou perdu 
short win(short gameState, char** grid)
{
    if (gameState != 2)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (grid[i][j] == '.')
                {
                    gameState = 0;
                    return gameState;
                }
            }
        }
        gameState = 1;
        return gameState;
    }
    else
    {
        return 2;
    }
}

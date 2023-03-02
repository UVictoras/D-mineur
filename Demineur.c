// Projet démineur en C sur console par Loan ALIX et Victor MARTINAN
//Les includes
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//Définition de notre variable globale
int size = 10;

//Pré définition des diverses fonctions
int TableSize();
void PrepareGrid(char** grid);
void PrintArrayGrid(char** grid);
void PlaceMine(char** grid);
void checkMine(char** grid, int x, int y);
void makePlay(char** grid, char** bombGrid, int positionX, int positionY);
void putFlag(char** grid, char** bombGrid, int positionX, int positionY);
short isMine(char** grid, int positionX, int positionY, short gameState);
short win(short gameState, char** grid);
void PlayMinesweeper(char** grid, char** grido);

int main()
{
    //La taille du tableau 
	size = TableSize();
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
	PlayMinesweeper(grid, grido);

	free(grid, grido);
}


//génération de La taille du tableau 
int TableSize()
{
	int Taille;
	printf("Veuillez saisir la taille du tableau :");
	scanf_s("%d", &Taille);
	return Taille;
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

//Fonction pour afficher la grille
void PrintArrayGrid(char** grid)
{
    for (int i = 0; i < size + 1; i++)
    {
        //Affichage des numéros de colonnes
        if (i < 10 && i != 1)
            printf(" ");

        (i == 0) ? (printf(" ")) : (printf("\033[0;37m%d", i));
        printf(" ");
    }
    printf("\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = -1; j < size + 1; j++)
        {   
            //Affichage des numéros de lignes
            printf(" ");
            if (j == size)
                printf("\033[0;37m%d", i + 1);
            else 
            { 
            //Affichage des éléments avec des couleurs différentes pour une meilleure visibilité en jeu    
                if (grid[i][j] == 'F')
                {
                    printf("\033[0;36m%c", grid[i][j]);
                }
                else if (grid[i][j] == '1')
                {
                    printf("\033[0;34m%c", grid[i][j]);
                }
                else if (grid[i][j] == '2')
                {
                    printf("\033[0;32m%c", grid[i][j]);
                }
                else if (grid[i][j] == '3')
                {
                    printf("\033[0;31m%c", grid[i][j]);
                }
                else if (grid[i][j] == '4')
                {
                    printf("\033[0;35m%c", grid[i][j]);
                }
                else if (grid[i][j] == '.')
                {
                    printf("\033[0;37m%c", grid[i][j]);
                }
                else if (grid[i][j] == 'B')
                {
                    printf("\033[0;33m%c", grid[i][j]);
                }
                else if (grid[i][j] == ' ')
                {
                    printf("%c", grid[i][j]);
                }
            }
            
            printf(" ");
        }
        printf("\n");
    }

}

//Fonction pour placer les mines de manière aléatoire en fonction de la taille du tableau 
void PlaceMine(char** grid)
{
    int MineNumber = size * size / 10;
    srand(time(NULL));

    int nbCase = size * size;
    int X;
    int Y;
    int k = 0;
    int** arr = (int**)malloc(sizeof(char*) * nbCase);
    for (int i = 0; i < nbCase; i++)
        arr[i] = i + 1;
    while (MineNumber > 0)
    {
        int coord = arr[(rand() % (nbCase - k)) + 1];
        X = coord / size;
        Y = coord % size;
        grid[X][Y] = 'B';

        for (int j = coord; j < nbCase - k - 1; j++)
        {
            arr[j] = arr[j + 1]; 
        }
        
        k++;
        MineNumber--;
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
void putFlag(char** grid,char** bombGrid, int positionX, int positionY)
{
    if (grid[positionX][positionY] != '.' && grid[positionX][positionY] != 'F') 
    {
        return;
    }
    else if(grid[positionX][positionY] == '.')
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
short win(short gameState , char** grid)
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

//Fonction pour jouer au démineur
void PlayMinesweeper(char** grid, char** grido)
{
    short gameState = 0;

    char** gameGrid= grid;
    char** realGrid= grido;

    char playerAction = ' ';
    int playerChoiceX = 0;
    int playerChoiceY = 0;
    int playCount = 0;

    PrepareGrid(gameGrid);
    PrepareGrid(realGrid);
    PlaceMine(realGrid);
    // Tant qu'on n'a ni perdu ni gagné 
	while (gameState == 0)
	{
		PrintArrayGrid(gameGrid);

        //Vérifier si on est au premier mouvement de l'utilisateur
		if (playCount == 0)
		{
			printf("\033[0;37mChoose a case to play :");
			scanf_s("%d %d", &playerChoiceX, &playerChoiceY);
			
            playerChoiceX--;
			playerChoiceY--;

            if (playerChoiceX >= size || playerChoiceY >= size)
                return;

            if (playerChoiceX < 0 || playerChoiceY < 0)
                return;

            //Si le premier choix du joueur est une bombe, on remplace cette bombe par un vide et on vérifie ce qu'il y a à côté
            if (realGrid[playerChoiceX][playerChoiceY] == 'B')
            {
                realGrid[playerChoiceX][playerChoiceY] = '.';

                for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        if (realGrid[i][j] != 'B')
                        {
                            checkMine(realGrid, i, j);
                        }
                    }
                }
            }
			makePlay(gameGrid, realGrid, playerChoiceX, playerChoiceY);
            playCount++;
		}
		else if (playCount != 0)
		{
			printf("\033[0;37mChoose a case to play (F for flag and P to show a case):");
			scanf_s("%c %d %d", &playerAction, 1, &playerChoiceX, &playerChoiceY);
			
            playerChoiceX--;
			playerChoiceY--;
			if (playerAction == 'F')
			{
				putFlag(gameGrid, realGrid, playerChoiceX, playerChoiceY);
                playCount++;
			}
			else if (playerAction == 'P')
			{
				makePlay(gameGrid, realGrid, playerChoiceX, playerChoiceY);
                playCount++;
                gameState = isMine(gameGrid, playerChoiceX, playerChoiceY, gameState);
			}

		}

		gameState = win(gameState, gameGrid);
		printf("\n");
	}
    //Gagné
    PrintArrayGrid(gameGrid);
    if (gameState == 1)
    {
        printf("\033[0;37mYou win ! Congratulations ! ");
    }
    //Perdu
    else if (gameState == 2)
    {
        printf("\033[0;37mSadly, you landed on a mine, might have more luck next time !");
    }
    return;
}

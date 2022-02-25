#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define abs(a) a > 0 ? a : -(a)
#define FIELD_SIZE 10


// struct to save every information about a player in the game
typedef struct Player {
    char name[100];
    int guesses;
    int enemyField[FIELD_SIZE][FIELD_SIZE];
    int field[FIELD_SIZE][FIELD_SIZE];
} Player;


void createNewPlayer(Player *player);
void randomPlacement(Player *player);
int _sum(int* array, int length);
int randomRange(int a, int b);
int placeShip(Player* player, int x1, int y1, int x2, int y2, int* boxes);
int humanPlay(Player* player, Player* enemy);
int computerPlay(Player* computerAI, Player* enemy, int difficulty);
int validatePosition(Player* player, int x1, int y1, int x2, int y2);
void printField(int field[FIELD_SIZE][FIELD_SIZE]);
void manualPlacement(Player* player);
void getPosition(int* x, int* y);
int alive(Player* player);
void saveScore(Player* player, Player* looser);




int main() {

    printf ("\t\tXXXXX    XXXX   XXXXXX  XXXXXX  XX      XXXXXX   XXXXX  XX  XX  XX  XXXX\n");
	printf ("\t\tXX  XX  XX  XX    XX      XX    XX      XX      XX      XX  XX  XX  XX  XX\n");
	printf ("\t\tXXXXX   XX  XX    XX      XX    XX      XXXX     XXXX   XXXXXX  XX  XXXX\n");
	printf ("\t\tXX  XX  XXXXXX    XX      XX    XX      XX          XX  XX  XX  XX  XX\n");
	printf ("\t\tXXXXX   XX  XX    XX      XX    XXXXXX  XXXXXX   XXXXX  XX  XX  XX  XX\n\n");
    printf("\t\t   ********Welcome to Battleship game by team Defenders********\n\n");
	printf ("Game Rules:\n\t1.Select positions for attack and ship placement as A1/B1 etc.\n ");
    printf ("\t2.Use capital Characters from A to J followed by numbers from 1 to 10. Do not use space in between\n");
    printf ("\t3.Enter 'y' only to play against computer. Entering any other character or number will lead to game against human player\n\n");
    printf("Please enter your name to begin: \n");

    Player p1;
    Player p2;
    createNewPlayer(&p1);
    createNewPlayer(&p2);
    scanf("%100s", p1.name);

    // diffficultyLevel contains the information about the difficulty. If it's 0, the match is against another human player
    int difficultyLevel = 0;
    char gameMode;

    printf("Do you want to play against computer?\n\t1- press 'y' for game against computer\n\t2- press 'n'to play against another player\n");
    scanf(" %c", &gameMode);
    if (gameMode == 'y') {
        strcpy(p2.name, "Computer");
        randomPlacement(&p2);
        printf("Please enter a difficulty. \n\t1 - Easy\n\t2 - Normal\n\t3 - Hard\n");
        scanf("%i", &difficultyLevel);
    } else {
        printf("Please enter the name of Player 2.\n");
        scanf("%100s", p2.name);
    }

    char random_ships;
    printf("%s, Do you want the ships to be placed randomly? (y/n)\n" , p1.name);
    scanf(" %c", &random_ships);
    if (random_ships == 'y') {
        randomPlacement(&p1);
        printField(p1.field);
    } else {
        printf("Please enter the ships for player: %s\n", p1.name);
        manualPlacement(&p1);
    }

    //only ask the user to fill in the board if the computer is not playing
    if (difficultyLevel == 0) {
        printf("%s, Do you want the ships to be placed randomly? (y/n)\n" , p2.name);
        scanf(" %c", &random_ships);
        if (random_ships == 'y') {
            randomPlacement(&p2);
            printField(p2.field);
        } else {
            printf("Please enter the ships for player %s\n", p2.name);
            manualPlacement(&p2);
        }
    }
   //play until one of the players is dead
    do {
        //ask the player to shot as long as he hits something
        do {
            printf("\n");
            printf("    It's %s 's turn!\n\n\    Enemy Board \n- - - - - - - - - - -\n", p1.name);
            printField(p1.enemyField);
        } while(humanPlay(&p1, &p2));

        //if player 2 is dead, player 1 can't play any more move
        if (!alive(&p2))
            break;

        if (difficultyLevel == 0){
            //ask player 2 to shot as long as he hits something
            do {
               printf("    It's %s 's turn!\n\n\    Enemy Board \n- - - - - - - - - - -\n", p2.name);
                printField(p2.enemyField);
            } while(humanPlay(&p2, &p1));
        }else
            while(computerPlay(&p2, &p1, difficultyLevel)); //computerAI plays as player 2

    } while(alive(&p1) && alive(&p2));


    Player* winner;
    Player* looser;
    if (alive(&p1)) {
        winner = &p1;
        looser = &p2;
    }else{
        winner = &p2;
        looser = &p1;
    }


    printf("!!!!!!!!%s has won!!!!!!!!\n\nScore Saved\n", winner->name);

    saveScore(winner,looser);
    return EXIT_SUCCESS;
}


/**
 * asks the player where to shot
 * updates the enemyField according to the shot
 * returns 1 if something is hit, 0 if not
 */
int humanPlay(Player* player, Player* enemy) {
    printf("Please enter the position where you want to attack:\n");
    int x, y;
    getPosition(&x, &y);
    player->guesses++;

    //if the enemy has any ship at that position
    if (enemy->field[x][y] == 1){
        //set the local information from the player about the enemy field, necessary for printing Hit positions
        player->enemyField[x][y] = 1;
        //set the enemy field to 0 at that position, necessary for alive function
        enemy->field[x][y] = 0;
        printf("\n");
        printf("    Hit!\n");
        printf("\n");
        //return if the enemy is still alive
        return alive(enemy);
    }
    printf("\n");
    printf("    Miss!\n");
    printf("\n");
    //update the local information about the enemy field, necessary for printing Missed positions
    player->enemyField[x][y] = 2;
    return 0;

}


/** creates a new player and fills the own and enemy field with 0 */

void createNewPlayer(Player *player) {
    player->guesses = 0;
    for (int y = 0; y < FIELD_SIZE; y++) {
        for (int x = 0; x < FIELD_SIZE; x++) {
            player->enemyField[x][y] = 0;
            player->field[x][y] = 0;
        }
    }
}


/** places player ships randomly */

void randomPlacement(Player* player) {
    srand(time(NULL));
    int boxes[] = {4, 3, 2, 1};
    int k = 0;
    //we just select random positions, until all the ships are placed
    while(_sum(boxes, 4) != 0) {
        //either choose a horizontal or vertical boat
        if (rand() & 1) {
            //choose a random size on the vertical
            int x = randomRange(0, 10);
            int y1 = randomRange(0, 10);
            int y2 = randomRange(0, 10);
            placeShip(player, x, y1, x, y2, boxes);
            continue;
        }
        //choose a random size on the horizontal
        int y = randomRange(0, 10);
        int x1 = randomRange(0, 10);
        int x2 = randomRange(0, 10);
        placeShip(player, x1, y, x2, y, boxes);
        k++;
        //there are some configurations where we can't find a solution, thats why we just reset the field after a specific amount of iterations
        if (k >= 1000000) {
            k = 0;
            //reset the boats
            for (int i = 0; i < 4; i++)
                boxes[i] = 4 - i;
            createNewPlayer(player);
        }
    }
}


int _sum(int* array, int length) {
    int sum = 0;
    //go through every element of the array and add it together
    for (int i = 0; i < length; i++)
        sum += array[i];
    return sum;
}

int randomRange(int a, int b) {
    return rand() % (b - a) + a;
}


/**
 * checks availability and size of the ship and places the ship if the position is valid.
 * reduces the number of available ship after a successful placement.
 * returns 1 if a ship is successfully placed, 0 if not
 */
int placeShip(Player* player, int x1, int y1, int x2, int y2, int* boxes) {
    //when the end position is lower then the start position we switch them
    int tmp;
    if (x2 < x1) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y2 < y1) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    int size;
    //when the boat is on the y-axis
    if (x1 == x2 && y1 != y2) {
        size = abs(y2 - y1);
    }

    //when the boat is on the x-axis
    if (x1 != x2 && y1 == y2) {
        size = abs(x2 - x1);
    }

    //if the ship is not on a straight line it is not valid
    if (x1 != x2 && y1 != y2)
        return 0;

    if (x1 == x2 && y1 == y2)
        return 0;
    //when the ship size is not valid. size= 4 means the ship has 5 boxes.
    if (size > 4)
        return 0;
    //when the player does not have any ships of this length left.
    if (boxes[size - 1] == 0)
        return 0;
    //when something is in the way we can't place the ship
    if (!validatePosition(player, x1, y1, x2, y2))
        return 0;
    //the position is valid, now we can remove one ship from the list
    boxes[size - 1]--;
    //set all the pieces of the ship on the field
    for (int x = x1; x <= x2; x++)
        for (int y = y1; y <= y2; y++)
            player->field[x][y] = 1;

    return 1;
}

/**
 * returns if the location of the ship is valid
 * checks if it's in the field, not colliding with any other ship and at least 1 box away from every other ship
 * return 1 if it's valid, 0 if not
 */
int validatePosition(Player* player, int x1, int y1, int x2, int y2) {
    //if one of the end/start position is out of the field, the position is not valid
    if (x1 < 0 || y1 < 0 || x1 >= FIELD_SIZE || y1 >= FIELD_SIZE)
        return 0;
    if (x2 < 0 || y2 < 0 || x2 >= FIELD_SIZE || y2 >= FIELD_SIZE)
        return 0;

    //check if the area of the ship is free
    for (int x = x1; x <= x2; x++)
        for (int y = y1; y <= y2; y++) {
            //if its not 0 there must be a ship and the position is not available
            if (player->field[x][y] != 0)
                return 0;

            //checks if it's at least one box away from every other ship
            if (x + 1 < FIELD_SIZE && player->field[x + 1][y] != 0)
                return 0;
            if (x - 1 >= 0 && player->field[x - 1][y] != 0)
                return 0;
            if (y + 1 < FIELD_SIZE && player->field[x][y + 1] != 0)
                return 0;
            if (y - 1 >= 0 && player->field[x][y - 1] != 0)
                return 0;
        }
    //if everything passes, the position is valid
    return 1;
}

/** prints the field to the stdout */
void printField(int field[FIELD_SIZE][FIELD_SIZE]) {
    // print the coordinate indicating numbers 1-10 as first row
    printf("  ");
    for (int x = 1; x <= FIELD_SIZE; x++)
        printf("%i ", x);
    printf("\n");

    for (int y = 0; y < FIELD_SIZE; y++) {
           printf("%c ", y + 'A'); //print the coordinate indicating Characters A-J as the first column

        for (int x = 0; x < FIELD_SIZE; x++) {
            //depending on the content of the field we want to print different values
            switch(field[x][y]) {
                case 0:
                    //the initial state of the field, thats why we just print dots
                    printf(". ");
                    break;
                case 1:
                    //if the field is 1, its a hit/boat
                    printf("X ");
                    break;
                case 2:
                    //if the field is 2, it's a miss
                    printf("0 ");
                    break;
            }
        }
        printf("\n");
    }
}


/**  *asks the player to place ships
     * print the successful placement on the stdout */

void manualPlacement(Player* player) {

    int boxes[] = {4, 3, 2, 1}; //a list to save what ships are available to place
    int flag = 0;  //a flag if every ship is placed

    do {
        //first we print the current state of the field, so that the player knows where is space available
        printField(player->field);
        //we then print availability of ships
        printf("Ships left: \n\t%i over 2 Boxes\n\t%i over 3 boxes\n\t%i over 4 boxes\n\t%i over 5 boxes\n", boxes[0], boxes[1], boxes[2], boxes[3]);
        int x1;
        int x2;
        int y1;
        int y2;
        printf("Please enter the beginning of the ship.\n");
        getPosition(&x1, &y1);

        printf("Please enter the end of the ship.\n");
        getPosition(&x2, &y2);

        if (placeShip(player, x1, y1, x2, y2, boxes))
            printf("\nSuccessfully placed the ship!\n\n");
        else {
            printf("\n!!!XXXX!!!Something went wrong, please check the position and length !!!XXXX!!!\n\n");
            //because something went wrong, we do not need to check if the ships are empty
            continue;
        }

        //goes through every element of the boxes and if any ship is still available, ask the user again for input
        flag = _sum(boxes, 4) != 0;
    } while (flag);
    //print the final state of the field
    printField(player->field);
}

/**
 * converts the user input to indices of the field array
 */
void getPosition(int* x, int* y) {
    char _y;
    scanf(" %c %i", &_y, x);
    *y = _y - 'A';
    *x -= 1;
}

/**
 * checks if the player is alive
 * returns 1 if true, 0 if not
 */
int alive(Player* player) {
    //go through every piece of the field and check if any of them is 1. If so, the player still has boats remaining
    for (int y = 0; y < FIELD_SIZE; y++)
        for (int x = 0; x < FIELD_SIZE; x++)
            if (player->field[x][y] == 1)
                return 1;
    return 0;
}

/**
 * makes one shot based on the difficultyLevel
 * updates the enemyField according to the shot
 * returns 1 if something is hit, 0 if not
 */
int computerPlay(Player* computerAI, Player* enemy, int difficulty) {
    computerAI->guesses++;
    //the chance of hitting something is =  difficulty / 4. If the difficulty is 1 the resulting chance is 25%, if it's 3 the chance is 75%
    int chance = difficulty / (float)4;
    float prob = (float)rand() / (float)RAND_MAX/1;

    //select a random position to shot
    int x = randomRange(0, 10);
    int y = randomRange(0, 10);
    //choose a new position where the computer has not shot yet
    while (computerAI->enemyField[x][y] == 1) {
        x = randomRange(0, 10);
        y = randomRange(0, 10);
    }


    if (prob < chance) {
        //if the chosen position is not a hit, we choose again, until we hit something
        while (enemy->field[x][y] == 0) {
            x = randomRange(0, 10);
            y = randomRange(0, 10);
        }
        printf("Computer Targeted\n\n");
        printf("   Hit! \n\n" );
        printf("!!!!!!!!the computer destroyed one of your ships!!!!!!!!!\n");
        //set the enemy field to 0 at that position
        enemy->field[x][y] = 0;
        //set the computer copy of the field to not hit there again
        computerAI->enemyField[x][y] = 1;
        return alive(enemy);
    }

    //if the computer hit something with the first guess
    if (enemy->field[x][y] == 1){
        //set the enemy field to 0 at that position
        enemy->field[x][y] = 0;
        computerAI->enemyField[x][y] = 1;
        printf("Computer Targeted\n\n");
        printf("   Hit! \n\n" );
        printf("!!!!!!the computer destroyed one of your ships!!!!!!\n");
        return alive(enemy);
    }

    //if the computer did not hit anything
    computerAI->enemyField[x][y] = 1;
    printf("   Computer Targeted\n\n");
    printf("   The computer missed!\n");
    return 0;
}

void saveScore(Player* winner,Player* looser) {
    FILE* scoreFile = fopen("score.txt", "a");
    if (!scoreFile)
        return;

    fprintf(scoreFile, "%s has won against %s with a score of: %i \n", winner->name,looser->name, winner->guesses);
    fclose(scoreFile);
}

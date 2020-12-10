#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Weapon information
typedef struct {
    char name[20];
    int max_damage;
} Weapon;

typedef enum{
    DEAD,
    ALIVE
}Status;

typedef struct{
    char name[20];
    int HP;
    int Exp;
    Weapon weapon;
    Status stat;
}Character;

typedef struct{
    Character *characters;
    int size; //tells how many characters there is in the game
}Game;

//Function for creating new characters. Used by functions load_game and add_characters.
void create_character(Game *game, char *name, int HP, char * weapon_name, int max_damage){
        Character *character;
        game->size += 1;
        int index = game->size -1;
        character = calloc(1,sizeof(Character)); 

        strcpy(character->name, name);
        strcpy(character->weapon.name, weapon_name);
        character->HP = HP;
        character->weapon.max_damage = max_damage;
        character->Exp= 0;
        if (HP > 0){
            character->stat = ALIVE;
        }
        else{
            character->stat = DEAD;
        }
    
        game->characters = realloc( game->characters, game->size* sizeof(Character));
        game->characters[index] = *character;
        free(character);
}

//Function for adding new characters in the game.
//Character can't have name of allready existing character or have 0 or lower HP
void add_characters(Game *game, char *name, int HP, char *weapon_name, int max_damage){
    int breaker = 1;
    for(int i= 0; i< game->size; i++){
        if(strcmp(game->characters[i].name,name)== 0){
            breaker = 0;
            break;
        }
    }
    if(breaker == 0){
        printf("Name %s is reserved! Choose another name.\n", name);
    }
    else if(HP <= 0){
        printf("There must be more than 0 hit points!\n");
    }
    else {
        create_character(game, name, HP, weapon_name, max_damage);
        printf("New character named %s added.\n", name);
    }
    
}


//Function for attacking
void attack(Game *game, char *attacker_name, char *victim_name){
    Character *attacker = NULL;
    Character *victim = NULL;

    for(int i= 0; i < game->size; i++){
        if(strcmp(game->characters[i].name, attacker_name)== 0){
            attacker = &game->characters[i];
        }
        else if(strcmp(game->characters[i].name, victim_name)== 0){
            victim = &game->characters[i];
        }
    }
    if(victim == NULL || attacker == NULL){
        printf("Invalid name of attacker or target!\n");
    }

    else if(victim->HP <= 0){
        printf("%s is dead. Can't attack!\n", victim->name);
    }

    else if(attacker->HP <= 0){
        printf("%s is dead. Can't attack!\n", attacker->name);
    }

    else{
        int damage;
        damage = rand() % attacker->weapon.max_damage;
        if (damage == 0){
            printf("%s tried to attack %s but missed.\n", attacker->name, victim->name);
        }
        else{
            victim->HP -= damage;
            attacker->Exp += damage;
            if (victim->HP <= 0){
                victim->HP = 0;
                victim->stat = DEAD;
                printf("%s attaced %s with %s by %d damage. %s died. %s gained %d \
experience points.\n", attacker->name, victim->name, attacker->weapon.name, damage, victim->name, attacker->name, damage - victim->HP);
            }
            else{
            printf("%s attaced %s with %s by %d damage. %s has %d hit points remaining. %s gained %d \
experience points.\n", attacker->name, victim->name, attacker->weapon.name, damage, victim->name, victim->HP, attacker->name, damage);
            }
        } 
    }  
} 

//Selection sort algorithm for the printing function under
void sort(Character *start, int size){

    int i,j;
    for(i = size-1; i>= 0; i--){
        for(j = i - 1; j >= 0; j--){
            if (start[j].Exp <start[i].Exp){
    
                Character *swap_i = &start[i];
                Character *swap_j = &start[j];

                Character help = *swap_i;
                *swap_i = *swap_j;
                *swap_j = help;
            }
        }
    }
}

//Function prints all the cracters descending order by Exp. Dead characters are at the end of the print
void print_characters(Game *game){
    Character living_characters[3];
    Character dead_characters[3];
    int count_living = 0;
    int count_died = 0;

    for(int i= 0; i< game->size; i++){
        if(game->characters[i].stat == ALIVE){
            living_characters[count_living] = game->characters[i];
            count_living ++;
        }
        else{
            dead_characters[count_died] = game->characters[i];
            count_died++;
        }
    }
    sort(living_characters, count_living);
    sort(dead_characters, count_died);

    for(int j= 0; j < count_living; j++){
        printf("%s, HP: %d, Exp: %d, weapon: %s, max damage of weapon: %d\n", living_characters[j].name, living_characters[j].HP,\
living_characters[j].Exp, living_characters[j].weapon.name, living_characters[j].weapon.max_damage);
    }

    if(count_died > 0){
        printf("\nDead characters:\n");
        for(int k=0; k < count_died; k++){
            printf("%s, HP: %d, Exp: %d, weapon: %s, max damage of weapon: %d\n", dead_characters[k].name, dead_characters[k].HP,\
dead_characters[k].Exp, dead_characters[k].weapon.name, dead_characters[k].weapon.max_damage);
        }
    }
}

//save_game saves the current situation on the game. Needed parameters are pointer to the current game 
// and the name of file, where the game is saved
void save_game(Game *game, char *name){
    FILE *file = fopen(name, "w");
    for(int i = 0; i< game->size; i++){
        fprintf(file,"%s %d %d %s %d\n", game->characters[i].name, game->characters[i].HP, game->characters[i].Exp,\
game->characters[i].weapon.name, game->characters[i].weapon.max_damage);
    }
    fclose(file);
    printf("Game was saved successfully\n");
}

// load_game loads the old game and replace the current situation in game with it
// parameters are pointer to current game and the name of the file where the old game
// is saved
void load_game(Game *game, char *name){
    FILE *file;
    if(!(file = fopen(name, "r"))){ // Check if the game file exist
        printf("There is no file named %s\n", name);
    }
    else{
        //FILE *file = fopen(name, "r");
        if(game->characters != NULL){
            free(game->characters);
            game->characters = NULL;
            game->size = 0;
        }

        char char_name[20];
        int HP;
        int Exp;
        Weapon weapon;

   
        while(fscanf(file, "%s %d %d %s %d", char_name, &HP, &Exp, weapon.name, &weapon.max_damage)!=EOF){
            create_character(game, char_name, HP, weapon.name, weapon.max_damage);
            game->characters[game->size-1].Exp = Exp;
        }
        fclose(file);
        printf("Game %s was loaded successfully\n", name);
    }
}

//Function for translating the commands given in main function
//If the command is Q return 1 and end the run. Otherwise return 1 and continue
int translate_line(Game* game, char* line){
    char command = line[0];
    char command_check = line[1];
    char name_1[20];
    int HP;
    char name_2[20];
    int max_damage;

    if(command_check == ' ' || command_check == '\n'){ //Check if the command is valid; One capita letter

        if(command == 'A'){
            if(sscanf(line, "%*c %s %d %s %d",  name_1, &HP, name_2, &max_damage) != 4){ //Check if thehre is right amount inputs and their type is right
                printf("Invalid input! There should be four parameters in input after command A. \
These parameters are \nname (string), HP (integer), the name of waepon (string) and max damage of weapon (integer) in this order.\n");
                return 0;
            }
            else{
                add_characters(game, name_1, HP, name_2, max_damage);
                return 0;
            }
        }

        else if(command == 'H'){ 
            if(sscanf(line,"%*c %s %s", name_1, name_2) != 2){ //Check if thehre is right amount inputs and their type is right
                printf("Invalid input! There should be two parameters in input after command H. \
These parameters are \nthe name of attacker (string) and the name of target (string).\n");
                return 0;
            }
            else{
                attack(game, name_1, name_2);
                return 0;
            }
        }

        else if(command =='L'){
            print_characters(game);
            return 0;
        }
        else if(command == 'W'){
            if (sscanf(line,"%*c %s", name_1) != 1){ //Check if thehre is right amount inputs and their type is right
                printf("Invalid input! There should be one parameter in input after command W. The parameter is the name of file (string).\n");
                return 0;
            }
            else{
                save_game(game, name_1);
                return 0;
            }
        }
        else if(command == 'O'){
            if(sscanf(line, "%*c %s", name_1) != 1){ //Check if thehre is right amount inputs and their type is right
                printf("Invalid input! There should be one parameter in input after command O. The parameter is the name of file (string).\n");
                return 0;
            }
            else{
                load_game(game, name_1);
                return 0;
            }
        }
        else if(command == 'Q'){ // Command Q ends the run
            return 1;

        }
        else{ 
            printf("%c is invalid command. Use command A, H, L, W, O or Q.\n", command);
            return 0;
        }
    }
    else{ //If the command in invalid format give this warning
        printf("Invalid Input! You didn't give the command which should be one capital letter.\n");
        return 0;
    }
}

//The main function which runs the program
int main(){
    Game game;
    game.characters = NULL;
    game.size = 0;

    int command = 0;
    char line[80];

    while(command != 1){
        fgets(line, 80, stdin);
        command = translate_line(&game, line);
    }

    free(game.characters);
    return 1;
}


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_INPUT 50
#define NUM_ROOMS 4
#define NUM_DIRECTIONS 4
#define POTION_HEAL 40
#define STARTING_POTIONS 2

enum Direction {
    NORTH = 0,
    SOUTH,
    EAST,
    WEST
};

enum CombatResult {
    COMBAT_WON,
    COMBAT_FLED,
    COMBAT_DIED,
    COMBAT_INPUT_CLOSED
};

typedef struct {
    int hp;
    int max_hp;
    int attack;
    int gold;
    int potions;
    int current_room;
    int previous_room;
    int has_key;
} Player;

typedef struct {
    char name[30];
    int hp;
    int attack;
    int gold_reward;
    int is_alive;
} Enemy;

typedef struct {
    char name[40];
    char description[200];
    int exits[NUM_DIRECTIONS];
    Enemy enemy;
    int has_chest;
} Room;

int read_line(char *buffer, size_t size);
void normalize_input(char *str);
void print_divider(void);
void initialize_game(Room dungeon[], Player *hero);
void display_room(const Room dungeon[], const Player *hero);
void display_stats(const Player *hero);
void display_help(void);
void handle_movement(Room dungeon[], Player *hero, int direction);
void handle_search(Room dungeon[], Player *hero);
enum CombatResult handle_combat(Player *hero, Enemy *monster);

int read_line(char *buffer, size_t size)
{
    size_t len;
    int ch;

    if (buffer == NULL || size < 2 ||
        size > (size_t)INT_MAX) {
        return 0;
    }

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[--len] = '\0';

        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[--len] = '\0';
        }

        return 1;
    }

    /*
       If the line did not fit, check whether more
       characters remain. Never leave a partial command
       waiting in the input stream.
    */
    ch = getchar();

    if (ch == '\n' || ch == EOF) {
        return 1;
    }

    while (ch != '\n' && ch != EOF) {
        ch = getchar();
    }

    buffer[0] = '\0';
    return -1;
}

void normalize_input(char *str)
{
    char *start = str;
    size_t i;

    while (*start != '\0' &&
           isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    i = strlen(str);

    while (i > 0 &&
           isspace((unsigned char)str[i - 1])) {
        str[--i] = '\0';
    }

    for (i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

void print_divider(void)
{
    printf("====================================================\n");
}

void initialize_game(Room dungeon[], Player *hero)
{
    int i;
    int j;

    hero->hp = 100;
    hero->max_hp = 100;
    hero->attack = 20;
    hero->gold = 0;
    hero->potions = STARTING_POTIONS;
    hero->current_room = 0;
    hero->previous_room = -1;
    hero->has_key = 0;

    for (i = 0; i < NUM_ROOMS; i++) {
        dungeon[i].name[0] = '\0';
        dungeon[i].description[0] = '\0';

        for (j = 0; j < NUM_DIRECTIONS; j++) {
            dungeon[i].exits[j] = -1;
        }

        dungeon[i].enemy.name[0] = '\0';
        dungeon[i].enemy.hp = 0;
        dungeon[i].enemy.attack = 0;
        dungeon[i].enemy.gold_reward = 0;
        dungeon[i].enemy.is_alive = 0;
        dungeon[i].has_chest = 0;
    }

    /* Room 0: Entrance */
    strcpy(dungeon[0].name, "Cobwebbed Entrance");
    strcpy(dungeon[0].description,
           "Dust covers the floor. A passage leads NORTH.");
    dungeon[0].exits[NORTH] = 1;

    /* Room 1: Armory */
    strcpy(dungeon[1].name, "Forgotten Armory");
    strcpy(dungeon[1].description,
           "Old weapon racks line the walls. "
           "Passages lead SOUTH, EAST and WEST.");
    dungeon[1].exits[SOUTH] = 0;
    dungeon[1].exits[EAST] = 3;
    dungeon[1].exits[WEST] = 2;

    strcpy(dungeon[1].enemy.name, "Scurrying Goblin");
    dungeon[1].enemy.hp = 35;
    dungeon[1].enemy.attack = 8;
    dungeon[1].enemy.gold_reward = 15;
    dungeon[1].enemy.is_alive = 1;

    /* Room 2: Treasury Vault */
    strcpy(dungeon[2].name, "Treasury Vault");
    strcpy(dungeon[2].description,
           "An old treasury filled with dust. "
           "The exit leads EAST.");
    dungeon[2].exits[EAST] = 1;
    dungeon[2].has_chest = 1;

    /* Room 3: Dragon's Lair */
    strcpy(dungeon[3].name, "Dragon's Lair");
    strcpy(dungeon[3].description,
           "Heat fills this enormous cavern. "
           "The only passage leads WEST.");
    dungeon[3].exits[WEST] = 1;

    strcpy(dungeon[3].enemy.name, "Ironscale Drake");
    dungeon[3].enemy.hp = 90;
    dungeon[3].enemy.attack = 18;
    dungeon[3].enemy.gold_reward = 200;
    dungeon[3].enemy.is_alive = 1;
}

void display_room(const Room dungeon[], const Player *hero)
{
    int current = hero->current_room;
    const Enemy *monster = &dungeon[current].enemy;

    print_divider();
    printf("LOCATION: %s\n", dungeon[current].name);
    printf("%s\n", dungeon[current].description);

    printf("\nHP: %d/%d | Attack: %d\n",
           hero->hp, hero->max_hp, hero->attack);

    printf("Gold: %d | Potions: %d | Key: %s\n",
           hero->gold, hero->potions,
           hero->has_key ? "YES" : "NO");

    if (monster->is_alive) {
        printf("Enemy: %s | Enemy HP: %d\n",
               monster->name, monster->hp);
    }

    print_divider();
}

void display_stats(const Player *hero)
{
    printf("\n--- PLAYER STATISTICS ---\n");
    printf("Health:  %d/%d\n", hero->hp, hero->max_hp);
    printf("Attack:  %d\n", hero->attack);
    printf("Gold:    %d\n", hero->gold);
    printf("Potions: %d\n", hero->potions);
    printf("Key:     %s\n",
           hero->has_key ? "Acquired" : "Not acquired");
}

void display_help(void)
{
    printf("\n--- COMMANDS ---\n");
    printf("go north / go south / go east / go west\n");
    printf("search       - Search for treasure\n");
    printf("stats        - Show player statistics\n");
    printf("look         - Display the current room\n");
    printf("map          - Show the dungeon map\n");
    printf("help         - Show commands\n");
    printf("exit         - Leave the game\n");

    printf("\nDuring combat:\n");
    printf("attack       - Attack the enemy\n");
    printf("potion       - Restore health\n");
    printf("flee         - Retreat to the previous room\n");
}

void handle_movement(Room dungeon[], Player *hero, int direction)
{
    int target;

    if (direction < 0 || direction >= NUM_DIRECTIONS) {
        printf("\nInvalid direction.\n");
        return;
    }

    target = dungeon[hero->current_room].exits[direction];

    if (target < 0 || target >= NUM_ROOMS) {
        printf("\nYou bump into a stone wall!\n");
        return;
    }

    if (target == 3 && !hero->has_key) {
        printf("\nThe Dragon's Lair is locked!\n");
        printf("Find the key in the Treasury Vault first.\n");
        return;
    }

    hero->previous_room = hero->current_room;
    hero->current_room = target;

    printf("\nYou enter: %s\n", dungeon[target].name);
}

enum CombatResult handle_combat(Player *hero, Enemy *monster)
{
    char input[MAX_INPUT];
    int result;

    if (hero == NULL || monster == NULL) {
        return COMBAT_INPUT_CLOSED;
    }

    printf("\n!!! COMBAT: %s !!!\n", monster->name);

    while (hero->hp > 0 && monster->hp > 0) {
        printf("\nYour HP: %d/%d | Enemy HP: %d\n",
               hero->hp, hero->max_hp, monster->hp);

        printf("Choose: attack, potion, flee > ");

        result = read_line(input, sizeof(input));

        if (result == 0) {
            return COMBAT_INPUT_CLOSED;
        }

        if (result < 0) {
            printf("Input too long. Try again.\n");
            continue;
        }

        normalize_input(input);

        if (strcmp(input, "flee") == 0) {
            if (hero->previous_room >= 0 &&
                hero->previous_room < NUM_ROOMS) {

                printf("\nYou retreat to safety!\n");
                hero->current_room = hero->previous_room;
                hero->previous_room = -1;
                return COMBAT_FLED;
            }

            printf("There is no safe retreat available!\n");
            continue;
        }

        if (strcmp(input, "potion") == 0) {
            int old_hp;
            int healed;

            if (hero->potions <= 0) {
                printf("You have no potions left!\n");
                continue;
            }

            if (hero->hp == hero->max_hp) {
                printf("Your health is already full!\n");
                continue;
            }

            old_hp = hero->hp;
            hero->hp += POTION_HEAL;

            if (hero->hp > hero->max_hp) {
                hero->hp = hero->max_hp;
            }

            healed = hero->hp - old_hp;
            hero->potions--;

            printf("You restored %d HP. Potions left: %d\n",
                   healed, hero->potions);
        } else if (strcmp(input, "attack") == 0) {
            printf("\nYou attack %s for %d damage!\n",
                   monster->name, hero->attack);

            monster->hp -= hero->attack;

            if (monster->hp < 0) {
                monster->hp = 0;
            }

            if (monster->hp == 0) {
                monster->is_alive = 0;
                hero->gold += monster->gold_reward;

                printf("\nYou defeated %s!\n", monster->name);
                printf("Gold earned: %d\n", monster->gold_reward);
                printf("Total gold: %d\n", hero->gold);

                return COMBAT_WON;
            }
        } else {
            printf("Invalid combat command. Use attack, potion, or flee.\n");
            continue;
        }

        /* The enemy responds after a successful player action. */
        printf("%s attacks for %d damage!\n",
               monster->name, monster->attack);

        hero->hp -= monster->attack;

        if (hero->hp < 0) {
            hero->hp = 0;
        }

        if (hero->hp == 0) {
            printf("\nYou have been defeated!\n");
            return COMBAT_DIED;
        }
    }

    return hero->hp > 0 ? COMBAT_WON : COMBAT_DIED;
}

void handle_search(Room dungeon[], Player *hero)
{
    int current = hero->current_room;

    if (dungeon[current].enemy.is_alive) {
        printf("\nDefeat the enemy before searching this room!\n");
        return;
    }

    if (!dungeon[current].has_chest) {
        printf("\nYou search the room but find nothing new.\n");
        return;
    }

    dungeon[current].has_chest = 0;

    printf("\nYou discover an ornate iron chest!\n");
    printf("Inside, you find the Dungeon Master Key and an elixir!\n");

    if (!hero->has_key) {
        hero->has_key = 1;
        printf("KEY ACQUIRED: You can now enter the Dragon's Lair!\n");
    }

    hero->max_hp += 20;
    hero->hp = hero->max_hp;

    printf("Maximum HP increased by 20.\n");
    printf("Your health has been fully restored!\n");
}

int main(void)
{
    Player hero;
    Room dungeon[NUM_ROOMS];
    char input[MAX_INPUT];
    int result;
    int quit = 0;

    initialize_game(dungeon, &hero);

    printf("\n");
    print_divider();
    printf("           C-DUNGEON CRAWLER - VERSION 3\n");
    print_divider();

    printf("Your quest: Find the key and defeat the dragon!\n");
    printf("You begin with %d HP and %d potions.\n",
           hero.hp, hero.potions);

    display_help();

    while (hero.hp > 0 && !quit) {
        int current;

        current = hero.current_room;

        if (current < 0 || current >= NUM_ROOMS) {
            printf("Internal error: invalid room. Ending game safely.\n");
            break;
        }

        display_room(dungeon, &hero);

        if (dungeon[current].enemy.is_alive) {
            enum CombatResult combat_result;

            combat_result = handle_combat(
                &hero, &dungeon[current].enemy);

            if (combat_result == COMBAT_INPUT_CLOSED) {
                printf("\nInput closed. Goodbye!\n");
                break;
            }

            if (combat_result == COMBAT_DIED || hero.hp <= 0) {
                break;
            }

            if (combat_result == COMBAT_FLED) {
                continue;
            }
        }

        if (hero.current_room == 3 &&
            !dungeon[3].enemy.is_alive) {

            print_divider();
            printf("\n          VICTORY! THE DRAGON IS DEFEATED!\n");
            printf("You have conquered the dungeon!\n");
            printf("Final HP: %d/%d\n", hero.hp, hero.max_hp);
            printf("Final gold: %d\n", hero.gold);
            printf("Potions remaining: %d\n", hero.potions);
            print_divider();
            break;
        }

        printf("\nWhat will you do? > ");

        result = read_line(input, sizeof(input));

        if (result == 0) {
            printf("\nInput closed. Goodbye!\n");
            break;
        }

        if (result < 0) {
            printf("Input too long. Please try again.\n");
            continue;
        }

        normalize_input(input);

        if (strcmp(input, "exit") == 0) {
            printf("\nYou leave the dungeon. Goodbye!\n");
            quit = 1;
        } else if (strcmp(input, "go north") == 0 ||
                   strcmp(input, "north") == 0) {
            handle_movement(dungeon, &hero, NORTH);
        } else if (strcmp(input, "go south") == 0 ||
                   strcmp(input, "south") == 0) {
            handle_movement(dungeon, &hero, SOUTH);
        } else if (strcmp(input, "go east") == 0 ||
                   strcmp(input, "east") == 0) {
            handle_movement(dungeon, &hero, EAST);
        } else if (strcmp(input, "go west") == 0 ||
                   strcmp(input, "west") == 0) {
            handle_movement(dungeon, &hero, WEST);
        } else if (strcmp(input, "search") == 0) {
            handle_search(dungeon, &hero);
        } else if (strcmp(input, "stats") == 0) {
            display_stats(&hero);
        } else if (strcmp(input, "look") == 0) {
            display_room(dungeon, &hero);
        } else if (strcmp(input, "help") == 0) {
            display_help();
        } else if (strcmp(input, "map") == 0) {
            printf("\n--- DUNGEON MAP ---\n");
            printf("                 [Dragon's Lair]\n");
            printf("                        |\n");
            printf("[Treasury Vault] -- [Armory] -- [Entrance]\n");
            printf("West of Armory: Treasury Vault\n");
            printf("East of Armory: Dragon's Lair (key required)\n");
        } else if (input[0] == '\0') {
            /* Ignore an empty command. */
        } else {
            printf("\nUnknown command. Type 'help' for assistance.\n");
        }
    }

    if (hero.hp <= 0) {
        print_divider();
        printf("\nGAME OVER! Your adventure has ended.\n");
        printf("You collected %d gold.\n", hero.gold);
        print_divider();
    }

    return 0;
}

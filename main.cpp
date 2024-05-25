#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define BOLDWHITE       "\x1b[1m\x1b[37m"
#define RESET 			"\x1b[0m"
#define RED 			"\x1b[31m"
#define GREEN 			"\x1b[32m"
#define RESET           "\x1b[0m"
#define BLACK           "\x1b[30m"
#define RED             "\x1b[31m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define MAGENTA         "\x1b[35m"
#define CYAN            "\x1b[36m"
#define WHITE           "\x1b[37m"
#define tKanan 185
#define lurusY 186
#define atasKanan 187
#define bawahKanan 188
#define bawahKiri 200
#define atasKiri 201
#define tBawah 202
#define tAtas 203
#define tKiri 204
#define lurusX 205
#define plus 206
#define candles 219
#define tick 179

// \033[2A keatas
// \033[11C kekanan
// \033[11D kekiri
// \033[1B kebawah

#define TABLE_SIZE 1000

void atas(int x){
	printf("\033[%dA", x);
}

void kanan(int x){
	printf("\033[%dC", x);
}

void kiri(int x){
	printf("\033[%dD", x);
}

void bawah(int x){
	printf("\033[%dB", x);
}

char currName[100];
char currEmail[100];
int currBalance = 100000;
char filename[] = "user.txt";
int baseBalance = 10000;
int xLen = 143;
int yLen = 38;
int startList = -1, lastList = 0;
int value, position, tp, sl, tempPos, tempTP, tempSL;
char outputError[100];
bool currTrade = 0, firstDraw = 0, which = 0; 
bool activeTrade = false;

void trade();

// HANDLE AUTHENTICATION 
typedef struct Account {				
    char name[50];
    char email[50];
    char password[50];
    int balance;
    struct Account *next;
} Account;

Account *AccountBank[TABLE_SIZE] = {NULL};

void menu();
bool isValidEmail(char email[], char password[], int choice);
void userMenu();

void fullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(100); // spy gk bug aowkaowkdokwoakdawd kasi waktu loading dlo
}

Account *createAccount(const char *name, const char *email, const char *password, int balance) {
    Account *newAccount = (Account *)malloc(sizeof(Account));

    strcpy(newAccount->name, name);
    strcpy(newAccount->email, email);
    strcpy(newAccount->password, password);
    newAccount->balance = balance;
    newAccount->next = NULL;

    return newAccount;
}

int hashFunction(const char *email) {
    int sum = 0;
    for (int i = 0; email[i] != '\0'; i++) {
        sum += email[i];
    }
    return sum % 26;
}

void insert(const char *name, const char *email, const char *password, int balance) {
    int index = hashFunction(email);
    Account *newAccount = createAccount(name, email, password, balance);

    if (AccountBank[index] == NULL) {
        AccountBank[index] = newAccount;
    } else {
        Account *temp = AccountBank[index];
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newAccount;
    }
}

void displayHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Account *temp = AccountBank[i];
        while (temp != NULL) {
            printf("Name: %s, Email: %s, Password: %s, Balance: %d\n", temp->name, temp->email, temp->password, temp->balance);
            temp = temp->next;
        }
    }
}

void fileToHash() {
    FILE *file;
    file = fopen(filename, "r");

    char name[50];
    char email[50];
    char password[50];
    int balance;

    while (fscanf(file, "%[^#]#%[^#]#%[^#]#%d\n", name, email, password, &balance) == 4) {
        insert(name, email, password, balance);
    }
}

void clr() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
  	COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(consoleHandle, &info);
}

void enter() {
    printf("\nPress enter to continue...\n");
    getch();
}

void error(const char *error) {
    printf("%s%s%s", RED, error, RESET);
}

bool checkingLen(int min, int max, int len) {
    if (len < min || len > max) {
        return false;
    }
    return true;
}

bool isValidEmail(char email[], char password[], int choice) {
    int len = strlen(email);
    int symbols = 0;
    int dotCount = 0;

    if (!checkingLen(6, 36, len)) {
        error("Your email length is > ");
        printf("%d", len);
        error("\nemail must be between 6 and 36 characters.\n");
        return false;
    }

    if (!(email[0] >= 'a' && email[0] <= 'z') && !(email[0] >= 'A' && email[0] <= 'Z')) {
        error("email must start with a letter.\n");
        return false;
    }

    for (int i = 0; i < len; i++) {
        if (email[i] == '@') {
        	if(email[i + 1] == '.'){
        		error("Please include the domain\n");
				return false;
			}
            symbols++;
        }
        if (email[i] == '.') {
            dotCount++;
        }
        if (!(email[i] >= 'a' && email[i] <= 'z') && !(email[i] >= 'A' && email[i] <= 'Z') &&
            !(email[i] >= '0' && email[i] <= '9') && email[i] != '@' && email[i] != '.') {
            error("email contains invalid characters.\n");
            return false;
        }
    }

    if (symbols != 1) {
        error("email must contain one '@'.\n");
        return false;
    }

    if (dotCount == 0) {
        error("email must contain at least one dot ('.').\n");
        return false;
    }

    for (int i = 0; i < len - 1; i++) {
        if ((email[i] == '.' && email[i + 1] == '.') || (email[i] == '@' && email[i + 1] == '@')) {
            error("email cannot have dots or '@' between.\n");
            return false;
        }
    }
	
    int atIndex = -1;
    int dotIndex = -1;

    for (int i = 0; i < len; i++) {
        if (email[i] == '@') {
            atIndex = i;
        } else if (email[i] == '.') {
            dotIndex = i;
        }
    }

    if (atIndex == -1 || dotIndex == -1 || atIndex > dotIndex) {
        error("email format is invalid.\n");
        return false;
    }

    if (strcmp(&email[dotIndex], ".com") != 0) {
        error("email must end with '.com'.\n");
        return false;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        Account *curr = AccountBank[i];
        while (curr != NULL) {
            if (choice == 0) {
                if (strcmp(email, curr->email) == 0 && strcmp(password, curr->password) == 0) {
                    strcpy(currName, curr->name);
                    strcpy(currEmail, curr->email);
                    currBalance = curr->balance;
                    userMenu();
                    return true;
                }
            } else {
                if (strcmp(email, curr->email) == 0) {
                	error("Email already taken\n");
                    return false;
                }
            }
            curr = curr->next;
        }
    }
	
	if(choice == 0){
	    error("Invalid Email or Password\n");
	    return false;
	} else {
		return true;
	}
}

char* getPassword() {
    int len = 100, i = 0;
    char ch, *password = (char*)malloc(len * sizeof(char));

    while (1) {
        ch = getch();

        if (ch == '\r' || ch == '\n') { 
            break;
        } else if (ch == '\b' && i > 0) { 
            printf("\b \b"); 
            i--;
        } else if (i < len - 1) { 
            password[i++] = ch;
            printf("*"); 
        }
    }

    password[i] = '\0'; 

    password = (char*)realloc(password, (i + 1) * sizeof(char));

    return password;
}

void login() {
    char email[100];
    char password[100];
    system("cls");
    do {
        printf("Input your email    > \n");
        printf("Input your password > \n");
        printf("(0 to %sEXIT%s)", RED, RESET);
        printf("\033[2A\033[11C");
        scanf("%s", email);
        if (email[0] == '0') menu();
        printf("\033[22C");
        strcpy(password, getPassword());
        system("cls");
    } while (email[0] != '0' && !(isValidEmail(email, password, 0)));
}


void guide() {
	system("cls");
	printf(BOLDWHITE "What Is Forex?\n" RESET);
	printf("\"Forex\", short for foreign exchange, is a global decentralized marketplace for trading currencies.\n");
	printf("In forex trading, participants buy one currency by selling another currency simultaneously, \nwith the aim of profiting from fluctuat ions in exchange rates. It is one of the largest and most liquid financial markets in the world,\noperating 24 hours a day, five days a week.\n\n");
	
	printf(BOLDWHITE "What Is Candle Stick?\n" RESET);
	printf("\"Candle Stick\" in charts are a type of financial chart used to represent price movements in trading markets, including forex.\n"); 
	printf("They visually display the open, high, low, and close prices for a specific period.\n");
	printf("If the close price is higher than the open price, the candle color will be green.\n");
	printf("then if the close price is less than the open price, the candle color will be red.\n");
	printf("shadow or tick is the highest and the lowest price of a candle.\n\n");
	
	printf(BOLDWHITE "What Is Position?\n" RESET);
	printf("\"Position\" in forex trading refers to where we want to enter the market at certain price point\n\n");
	
	printf(BOLDWHITE "What Is Long?\n" RESET);
	printf("\"Long\" in forex trading refers to a trading position where a trader buys a currency pair\nwith the expectation that its value will increase over time.\n\n");
	
	printf(BOLDWHITE "What Is Short?\n" RESET);
	printf("\"Short\" in forex trading refers to a trading position where a trader sells a currency pair\nwith the expectation that its value will decrease over time.\n\n");
	
	printf(BOLDWHITE "What is Take Profit?\n" RESET);
	printf("\"Take Profit\" in forex trading refers to where we will leave the market at the specified price point\nour position will automatically closed after the market price hit our take profit price.\n\n");
	
	printf(BOLDWHITE "What Is Stop Loss?\n" RESET);
	printf("\"Stop Loss\" in forex trading refers to where we will leave the market at the specified price point.\nit's used to make sure that we don't loss all of our money and many more\nsame like take profit, our position will automatically closed after the market price hit our take profit price\n\n");

	
	char thanks[] = "Thanks to 24-2";
	int len = strlen(thanks);
	int asc = 40, i = 0;
	printf(MAGENTA);
	while(i < len){
		printf("%c", asc);
		if(thanks[i] == asc){
			i++;
			asc = 40;
			printf(" ");
		} if(thanks[i] == ' '){
			printf("\b  ");
			i++;
		}
		asc++;
		printf("\b");
		Sleep(1);
	}
	printf(RESET);
	
	enter();
	system("cls");
	userMenu();
}

bool isValidUsername(char username[]) {
    int len = strlen(username);

    if (!checkingLen(1, 20, len)) {
        error("Username must be between 1 - 20\n");
        return false;
    }

    for (int i = 0; i < len; i++) {
        if (!(username[i] >= 'A' && username[i] <= 'Z') &&
            !(username[i] >= 'a' && username[i] <= 'z') &&
            !(username[i] >= '0' && username[i] <= '9')) {
            error("Username must not have any symbols\n");
            return false;
        }
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        Account *curr = AccountBank[i];
        while (curr != NULL) {
            if (strcmp(username, curr->name) != 0) {
                strcpy(currName, curr->name);
                strcpy(currEmail, curr->email);
                currBalance = curr->balance;
                return true;
            }
            curr = curr->next;
        }
    }

    error("Username already taken !\n");
    return false;
}

bool isValidPassword(char password[]){
	int len = strlen(password);
	int hasAlp = 0, hasNum = 0;
	
	
	if(!checkingLen(8, 36, len)){
		clr();
		error("Length must be 8 - 36\n");
		return false;
	}
	
	for (int i = 0; password[i] != '\0'; i++) {
        if (!(password[i] >= 'a' && password[i] <= 'z') && !(password[i] >= 'A' && password[i] <= 'Z') 
		&& !(password[i] >= '0' && password[i] <= '9')) {
			clr();
        	error("Password contains symbols\n");
            return false;
        } 
    }
    
    return true;
}

void registering() {
    char username[100];
    char email[100];
    char password[100];
  
    do {
        printf("Input your Username > ");
        printf("\n(0 to %sEXIT%s)", RED, RESET);
        printf("\033[1A\033[11C");
        scanf("%s", username);
        clr();
    } while (!isValidUsername(username));
  
    if (username[0] == '0')
        menu();
    
    do {
        printf("Your Username is %s%s%s\n", GREEN, username, RESET);
        printf("Input your Email > ");
        printf("\n(0 to %sEXIT%s)", RED, RESET);
        printf("\033[1A\033[8C");
        scanf("%s", email);
        clr();
    } while (!isValidEmail(email, "", 1));
  
    if (email[0] == '0')
        menu();
        
    do {
    	printf("Your Username is %s%s%s\n", GREEN, username, RESET);
    	printf("Your Email is %s%s%s\n", GREEN, email, RESET);
        printf("Input your Password > ");
    	strcpy(password, getPassword());
	} while(!isValidPassword(password));
	
	
	FILE *file = fopen(filename, "a");
	
	fprintf(file, "%s#%s#%s#%d\n", username, email, password, baseBalance);
	fclose(file);
	
	printf("\n%sYour register is complete!%s\n", GREEN, RESET);
	
	enter();
    menu();
}

void exit(){
	system("cls");
	printf(MAGENTA "Meta Fi\n\n" RESET);
	printf(GREEN "Created with 24-2 Squad\n\n" RESET);
	printf(RESET "Thank you 24-2 !\n\n");
}

void menu() {
    int choice;
    system("cls");
    do {
    char title[][300] = {
		"______________  _______  _________  ________  ________ ___          ___    ___       _______  ___   ___                   _______",
        "|\\   _ \\  _   \\|\\  ___ \\|\\___   ___\\\\   __  \\|\\  _____\\\\  \\        |\\  \\  /  /|     /  ___  \\|\\  \\ |\\  \\                 /  ___  \\",  
        "\\ \\  \\\\\\__\\ \\  \\ \\   __/\\|___ \\  \\_\\ \\  \\|\\  \\ \\  \__/\\ \\  \\        \\ \\  \\/  / /    /__/|_/  /\\ \\  \\\\_\\  \\  ____________ /__/|_/  /|",  
        " \\ \\  \\\\|__| \\  \\ \\  \\_|/__  \\ \\  \\ \\ \\   __  \\ \\   __\\\\ \\  \\       \\ \\    / /     |__|//  / /\\ \\______  \\|\\____________\\__|//  / /",
        "  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\  \\ \\  \\ \\ \\  \\ \\  \\ \\  \\_| \\ \\  \\       /     \\/          /  /_/__\\|_____|\\  \\|____________|   /  /_/__",
        "   \\ \\__\\    \\ \\__\\ \\_______\\  \\ \\__\\ \\ \\__\\ \\__\\ \\__\\   \\ \\__\\     /  /\\   \\         |\\________\\     \\ \\__\\               |\\________\\",
        "    \\|__|     \\|__|\\|_______|   \\|__|  \\|__|\\|__|\\|__|    \\|__|    /__/ /\\ __\\         \\|_______|      \\|__|                \\|_______|",
        "                                                                  |__|/  \\|__|                                                                  "
	};

    printf(BOLDWHITE);
    for(int i = 0; i < 8; i++){
    	printf("%s\n", title[i]);
	}
	printf(RESET);
                                                                                                                                    
        printf("1. Log in to trade\n");
        printf("2. Register Account\n");
        printf("3. Exit Application\n");
        printf(">> ");
        scanf("%d", &choice); getchar();
		system("cls");

        switch (choice) {
            case 1:
            	system("cls");
                login();
                break;
            case 2:
                registering();
                break;
            case 3:
            	system("cls");
            	exit();
                break;
            default:
                error("Please choose between 1 - 3\n");
                menu();
        }
    } while (choice < 1 || choice > 3);
    
}

// HANDLE GRAPH

struct candle {
    int open;
    int close;
    int highest;
    int lowest;
};

struct Node {
    struct candle data;
    struct Node* prev;
    struct Node* next;
};

struct Node* head = NULL;
struct Node* tail = NULL;

struct Node* createNode(struct candle data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void pushTail(struct candle data) {
    struct Node* newNode = createNode(data);
    if (tail == NULL) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

void displayList() {
    struct Node* current = head;
    while (current != NULL) {
        printf("Open: %d, Close: %d, Highest: %d, Lowest: %d\n",
               current->data.open, current->data.close,
               current->data.highest, current->data.lowest);
        current = current->next;
    }
}

void freeList() {
    struct Node* current = head;
    while (current != NULL) {
        struct Node* temp = current;
        current = current->next;
        free(temp);
    }
    head = tail = NULL;
}

bool chance(int percent){
	if(rand() % 100 + 1 < percent) return true;
	return false;
}

void add(int num){
	int minPlus, close, open, highest, lowest, prevHighest, prevLowest;
	int tHigh, tLow;
	prevHighest = prevLowest = 0;
	for(int i = 0; i < num; i++){
		struct Node* current = tail;
//		rand() % 2 == 0 ? minPlus = -1 : minPlus = 1;
		head == NULL ? open = (15 + rand() % (20 - 15 + 1)) * 10 : open = tail->data.close;
		
		// handle close
		if(prevLowest >= 330) open = 330;
		
		if (open < 10) {
		    close = open + ((rand() % 6) + 1) * 10;
		} else {
		    if (chance(10)) {
		        close = open;
		    } else if (chance(50)) {
		        close = open + ((rand() % 6) + 1) * 10;
		    } else {
		        close = open - ((rand() % 6) + 1) * 10;
		    }
		}
		
		// handle highest
		if(close > open){
			if(chance(30)){
				highest = close;
			} else {
				highest = close + (rand() % 4) * 10;
			}
		} else {
			if(chance(30)){
				highest = open;
			} else {
				highest = open + (rand() % 4) * 10;
			}
		}	
		if(highest < 0) highest *= -1;
		
		// handle lowest
		if(close >= open){
			if(chance(30)){
				lowest = open;
			} else {
				lowest = open - (rand() % 4) * 10 ;
			}
		} else {
			if(chance(30)){
				lowest = close;
			} else {
				lowest = close - (rand() % 4) * 10;
			}
		}
		
		if(lowest < 0) lowest *= -1;
		lastList++;
		
		prevHighest = highest;
		prevLowest = lowest;
		
		if(open <= 0) {
			open *= -1; 
			lowest = 0;	
		}
		
		if(close < 0) {
			close *= -1; 
			lowest = 0;	
		}
		
		if(close > open){
			tHigh = close;
			tLow = open;
		} else {
			tHigh = open;
			tLow = close;
		}
		
		if(lowest > tLow) lowest = tLow;
		if(highest < tHigh) highest = tHigh;
		
		pushTail((struct candle){open,close,highest,lowest});
		
	}
}

void initOutputError(char errors[100]){
	strcpy(outputError, errors);
}

// Handle History
typedef struct History {
    char* username;
    int value;
    int position;
    int tp;
    int sl;
    int reward;
    struct History* next;
} History;

typedef struct PriorityQueue {
    History* head;
} PriorityQueue;

History* createHistory(char* username, int value, int position, int tp, int sl, int reward) {
    History* newHistory = (History*)malloc(sizeof(History));
    newHistory->username = username;
    newHistory->value = value;
    newHistory->position = position;
    newHistory->tp = tp;
    newHistory->sl = sl;
    newHistory->reward = reward;
    newHistory->next = NULL;
    return newHistory;
}

PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    pq->head = NULL;
    return pq;
}

void enqueue(PriorityQueue* pq, char* username, int value, int position, int tp, int sl, int reward) {
    History* newHistory = createHistory(username, value, position, tp, sl, reward);
    
    if (pq->head == NULL || pq->head->reward < reward) {
        newHistory->next = pq->head;
        pq->head = newHistory;
    } else {
        History* current = pq->head;
        while (current->next != NULL && current->next->reward >= reward) {
            current = current->next;
        }
        newHistory->next = current->next;
        current->next = newHistory;
    }
}

PriorityQueue* pq = createPriorityQueue();

History* dequeue(PriorityQueue* pq) {
    if (pq->head == NULL) {
        printf("Priority queue is empty.\n");
        return NULL;
    }
    History* temp = pq->head;
    pq->head = pq->head->next;
    temp->next = NULL;
    return temp;
}

void displayHistory(PriorityQueue* pq) {
    History* current = pq->head;
    printf(GREEN "%s's Data\n\n\n" RESET, currName);
    int i = 1;
    while (current != NULL) {
    	if(strcmp(current->username, currName) == 0){
    		printf("No: %d\n", i++);
    		printf("Value       : %d\n", current->value);
    		printf("Position    : %d\n", current->position);
    		printf("Take Profit : %d\n", current->tp);
    		printf("Stop Loss   : %d\n", current->sl);
    		printf("Reward      : ");
			current->reward < 0 ? printf(RED) : printf(GREEN); 
    		printf("%d\n\n\n\n" RESET, current->reward);
    		
		}
        current = current->next;
    }
    
    enter();
}

void draw(){
	//top
	clr();
	printf("\n%c", atasKiri);
    for (int i = 0; i < xLen + 1; i++) {
        printf("%c", lurusX);
    }
    printf("%c\n", atasKanan);
	
	bool reset = 0;
	
	// mid
	int num = 330;
    for (int i = 0; i < yLen - 5; i++) {
		int highCandle, lowCandle, lowest, highest, open,close, prevClose, prevHighCandle, prevColor, prevLowCandle;
        printf("%c", lurusY);
        struct Node* current = head;
        
        int currList = 0;
		
		while(currList < startList){
			current = current->next;
			currList++;
		}
		
		if(currList >= xLen - 7) startList++;
			
		int tailHighCandle, tailLowCandle;
        for (int j = 0; j < xLen; j++) {
        	// container
			
			lowest = current->data.lowest;
			highest = current->data.highest;
			open = current->data.open;
			close = current->data.close;
			
			if(current->data.open > current->data.close){
				highCandle = current->data.open;
				lowCandle = current->data.close;
			} else {
				highCandle =  current->data.close;
				lowCandle =  current->data.open;
			}
			
			
			if(tail->data.close > tail->data.open){
				tailHighCandle = tail->data.close;
				tailLowCandle = tail->data.open;
			} else {
				tailHighCandle = tail->data.close;
				tailLowCandle = tail->data.open;
			}
			
			
			if(!activeTrade && position <= tailHighCandle && position >= tailLowCandle){
				activeTrade = true;
				strcpy(outputError, "Trade currently active");
			}
			
			if(j > xLen - 6){ // data kanan
				if(j == xLen - 5) printf("%c", lurusY);
				if(j == xLen - 4) {
					printf(" %-3d ", num);
				}
			} else if(num <= highest && num >= lowest && current->next != NULL){ // graph
				if (num == lowest == highest == open == close){
					printf(RED "%c", candles);	
				} else if(num > highCandle && num <= highest){
					highCandle > prevHighCandle ? printf(GREEN) : printf(RED);
					printf("%c" RESET, tick);
				} else if(num < lowCandle && num >= lowest){
					highCandle > prevHighCandle ? printf(GREEN) : printf(RED);
					printf("%c" RESET, tick);
				} else {
					if(highCandle > prevHighCandle || current == head ){
						printf("%s", GREEN);
						prevColor = 0;
					} else {
						printf("%s", RED);
						prevColor = 1;
					}
					
					printf("%c" RESET, candles);
				}
			} else { // Take Profit & Stop Loss
				if(tp == num ){
					printf("%s", GREEN);
					if(firstDraw) Sleep(1);
				} else if(sl == num ){
					printf("%s", RED);
					if(firstDraw) Sleep(1);
				} else if (position == num){
					printf("%s", MAGENTA);
					if(firstDraw) Sleep(1);
				}
				
            	printf("-");
			}
			
			prevHighCandle = highCandle;
			prevLowCandle = lowCandle;
        	prevClose = close; // mau pake prev tapi ngebug
        	currList++;
			if(current->next != NULL) current = current->next;
			printf("%s", RESET);
        }
        
		num -= 10;	
        printf("%c\n", lurusY);
        int reward = 0;
		int target;
		if (which == 1) {
		    target = tp;
		} else if (which == 2) {
		    target = sl;
		}
		
//		if(activeTrade){
//			if(tp >= tailHighCandle && tp <= tailLowCandle){
//				reward = (value * abs(close - position)) / 100;
//			} else if(sl >= tailHighCandle && sl <= tailLowCandle){
//				reward = (value * abs(close - position)) / 100;
//			}
//		}
		
		// check trade
		if (activeTrade && ((tp >= tailHighCandle && tp <= tailLowCandle) || (sl >= tailHighCandle && sl <= tailLowCandle))) {
			if(which == 1){
				if((tp >= tailHighCandle && tp <= tailLowCandle)){
					reward = (value * abs(close - position)) / 100;
				} else if (sl >= tailHighCandle && sl <= tailLowCandle){
					reward = -(value * abs(close - position)) / 100;
				}
			} else if(which == 2){
				if((tp >= tailHighCandle && tp <= tailLowCandle)){
					reward = -(value * abs(close - position)) / 100;
				} else if (sl >= tailHighCandle && sl <= tailLowCandle){
					reward = (value * abs(close - position)) / 100;
				}
			}
		    
			activeTrade = false;
		    currTrade = 0;
		    firstDraw = 0;
		    enqueue(pq, currName, value, position, tp, sl, reward);
		    sl = tp = position = value = tempSL = tempTP = tempPos = 0;
		    reset = 1;
		    currBalance += reward;
		    strcpy(outputError, "Trade ended");
		}
    }
    
    firstDraw = 0;
    printf("%c", tKiri);
    for (int i = 0; i < xLen + 1; i++) {	
        printf("%c", lurusX);
    }
    printf("%c", tKanan);
    
    printf("\n");
    
    // data bawah
	printf("%c" MAGENTA " Welcome to METAFI, " GREEN "%-18s " RESET "%c   Value          : %-8d    "GREEN"TP : %-4d"RESET"                                                %c              %c\n"  ,
	lurusY,currName,lurusY,value,tempTP,lurusY,lurusY); 
	
	printf("%c 1. New Day   3. Next  5. Short        %c   Position       : %-4d        "RED"SL : %-4d"RESET"                                                %c" GREEN " $%-7d" RESET "     %c\n" , lurusY, lurusY, tempPos, tempSL, lurusY,currBalance ,lurusY);
	
	printf("%c 2. Previous  4. Long  6. Logout " MAGENTA ">>" RESET "    %c   " RED "%-50s" RESET "                                    %c              %c\n", lurusY, lurusY,outputError ,lurusY, lurusY);
	
	// bot
    printf("%c", bawahKiri);
    for (int i = 0; i < xLen + 1; i++) {
        printf("%c", lurusX);
    }
    printf("%c\n", bawahKanan);
    
    if(reset){
    	draw();
    	reset = 0;
	}
    
}

void userMenu() {
    int choice = 0;

    do {
        printf("%s%s%s - User's Menu\n", GREEN, currName, RESET);
        printf("1. Start Trade\n");
        printf("2. Trade History\n");
        printf("3. Guide\n");
        printf("4. Log out\n");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
            	system("cls");
                trade();
                break;
            case 2:
            	clr();
            	system("cls");
                displayHistory(pq);
                system("cls");
                userMenu();
                break;
            case 3:
                guide();
                break;
            case 4:
            	system("cls");
                menu();
                break;
            default:
            	clr();
                error("Please choose between 1-4\n");
                userMenu();
        }
    } while (choice < 1 || choice > 4);
    
    
}

void goLong(){
	activeTrade = false;
	if(currTrade) return;
	
	struct Node* current = tail;
	
	int highCandle, lowCandle, highest, lowest;
	if (tail->data.open > tail->data.close) {
		highCandle = tail->data.open;
		lowCandle = tail->data.close;
	} else {
		highCandle = tail->data.close;
		lowCandle = tail->data.open;
	}
	
	highest = tail->data.highest;
	lowest = tail->data.lowest;
	
	// Handle Value
	do {
		atas(3); kanan(61);
		scanf("%d", &value); getchar();
		atas(1); kanan(61);
		if(!(value >= 10 && value <= currBalance)) {
			strcpy(outputError, "Value must > 10 and < Current Balance");
			draw();
			atas(1);
		}
		 
	} while(!(value > 10 && value < currBalance));
	bawah(1);
	
	strcpy(outputError, "");
	draw();
	atas(3); kanan(61);
	
	do {
		scanf("%d", &tempPos); getchar();
		atas(1); kanan(61);
		if(!(tempPos >= 20 && tempPos < 330)) {
			strcpy(outputError, "Position must > 20 & < 330");
			draw();
			atas(3); kanan(61);
		}
	} while(!(tempPos > 20 && tempPos < 330));
	
	strcpy(outputError, "");
	draw();
	atas(3); kanan(61);
	
	atas(1); kanan(17);
	do	{
		scanf("%d", &tempTP); getchar();
		atas(1); kanan(78);
		if((tempTP < tempPos) || (tempTP < 10 || tempTP > 330 )){
			strcpy(outputError, "TP must => 10 & <= 330 also > Position ");
			draw();
			atas(4); kanan(78);
		}
	} while((tempTP < tempPos) || (tempTP < 10 || tempTP > 330 ));

	strcpy(outputError, "");
	draw();
	atas(4); kanan(78);
	printf("%-3d", tempTP);
	bawah(1); kiri(3);
	
	do {
		scanf("%d", &tempSL); getchar();
		atas(1); kanan(78);
		if((tempSL > tempPos) || (tempSL < 10 || tempSL > 330)){
			strcpy(outputError, "SL must => 10 & <= 330 also < Position ");
			draw();
			atas(3); kanan(78);
		}
	} while((tempSL > tempPos) || (tempSL < 10 || tempSL > 330));
	
	strcpy(outputError, "");
	draw();
	atas(3); kanan(78);
	tp = tempTP; sl = tempSL; position = tempPos;
	tp = tp - (tp % 10); // auto convert kelipatan 10
	sl = sl - (sl % 10); // auto convert kelipatan 10	
	firstDraw = 1;
	currTrade = 1;
	which = 1;
	draw();
}

void goShort(){
	activeTrade = false;
	if(currTrade) return;
	
	struct Node* current = tail;
	
	int highCandle, lowCandle;
	if (tail->data.open > tail->data.close) {
		highCandle = tail->data.open;
		lowCandle = tail->data.close;
	} else {
		highCandle = tail->data.close;
		lowCandle = tail->data.open;
	}
	
	// Handle Value
	do {
		atas(3); kanan(61);
		scanf("%d", &value); getchar();
		atas(1); kanan(61);
		if(!(value >= 10 && value <= currBalance)) {
			strcpy(outputError, "Value must > 10 and < Current Balance");
			draw();
			atas(1);
		}
		 
	} while(!(value > 10 && value < currBalance));
	bawah(1);
	
	strcpy(outputError, "");
	draw();
	atas(3); kanan(61);
	
	do {
		scanf("%d", &tempPos); getchar();
		atas(1); kanan(61);
		if(!(tempPos >= 20 && tempPos < 330)) {
			strcpy(outputError, "Position must >= 20 & < 330");
			draw();
			atas(3); kanan(61);
		}
	} while(!(tempPos > 20 && tempPos < 330));
	
	strcpy(outputError, "");
	draw();
	atas(3); kanan(61);
	
	atas(1); kanan(17);
	do	{
		scanf("%d", &tempTP); getchar();
		atas(1); kanan(78);
		if((tempTP > tempPos) || (tempTP < 10 || tempTP > 330 )){
			strcpy(outputError, "TP must => 10 & <= 330 also < Position");
			draw();
			atas(4); kanan(78);
		}
	} while((tempTP > tempPos) || (tempTP < 10 || tempTP > 330 ));

	strcpy(outputError, "");
	draw();
	atas(4); kanan(78);
	printf("%-3d", tempTP);
	bawah(1); kiri(3);
	
	do {
		scanf("%d", &tempSL); getchar();
		atas(1); kanan(78);
		if((tempSL < tempPos) || (tempSL < 10 || tempSL > 330 )){
			strcpy(outputError, "SL must => 10 & <= 330 also > Position ");
			draw();
			atas(3); kanan(78);
		}
	} while((tempSL < tempPos) || (tempSL < 10 || tempSL > 330 ));
	
	strcpy(outputError, "");
	draw();
	atas(3); kanan(78);
	tp = tempTP; sl = tempSL; position = tempPos;
	tp = tp - (tp % 10); // auto convert kelipatan 10
	sl = sl - (sl % 10); // auto convert kelipatan 10	
	firstDraw = 1;
	currTrade = 1;
	which = 2;
	draw();
}

void trade() {
	int choice;
	draw();
	printf("\033[2A\033[37C");
	scanf("%d", &choice); getchar();
	
	switch(choice){
		case 1:
			if(lastList > xLen - 6){
				startList++;
			}
			add(1);
			Sleep(10);
			break;
		case 2:
			startList--;
			if(startList < -1) startList = -1;
			break;
		case 3:
			startList++;
			if(startList > lastList - 2) startList = lastList - 2;
			break;
		case 4:
			if(activeTrade) {
				strcpy(outputError, "Theres trade currently active");
			} else {
				goLong();
			}
			break;
		case 5:
			if(activeTrade) {
				strcpy(outputError, "Theres trade currently active");
			} else {
				goShort();
			}
			break;
		case 6: 
			menu();
			break;
		default: 
			break;
	}
	
	if(choice != 6) trade();
	
}

int main() {
	system(" ");
	srand(time(NULL));
		
	add(20);
    fullscreen();
    fileToHash();
    menu();
//  	userMenu();
//	trade();
//	draw();
//	displayList();
	
    return 0;
}

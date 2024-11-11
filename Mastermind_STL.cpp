/******************************************
* Author    : Bryan Estrada               *  
* Teacher   : Dr. Mark Lehr               *  
* Class     : CSC-17C                     *
* Assignment: Project #1                  *  
* Title     : Mastermind with STL Library *
******************************************/

//Libraries
#include <iostream>
#include <cstdlib>   // Random Function Library
#include <ctime>     // Time Library
#include <string>
#include <set>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <utility>
#include <algorithm>
using namespace std;

//Structures
/************************************************************
* STRUCT: GameResult
*____________________________________________________________
* PURPOSE:
*    Holds the result of a single game of Mastermind, 
*    including game settings and outcome.
*
* MEMBERS:
*    - int codeLength: Length of the code used in the game.
*    - char duplicateSetting: Character indicating whether 
*                             duplicates are allowed ('Y' for
*                             yes, 'N' for no).
*    - isWin (bool): True if the player won the game, false 
*                    if lost.
*
* CONSTRUCTOR:
*    GameResult(int len, char dup, bool win):
*       Initializes a GameResult with given code length, 
*       duplicate setting, and win/loss outcome.
************************************************************/
struct GameResult {
    int codeLength;
    char duplicateSetting;
    bool isWin; // true if user won, false if lost
    GameResult(int len, char dup, bool win)  {
        codeLength = len; 
        duplicateSetting = dup; 
        isWin = win;
    }
};

//Function prototypes
void setupGame();
char getDuplicateChoice();
int getCodeLength();
void genCode(int, list<char>&, char);
void printCode(const list<char>&);
void hint(const list<char>&, const list<char>&);
void showGameOverMessage(const list<char>&);
void showInstructions();
void validInput(const string&, bool&, const int&);
void compareGuess(list<char>&, const string&, const list<char>&, bool&, 
                  stack<int>&, const int&, const char&, queue<GameResult>&);
void exitingGame(bool&);
void newGame(char&);
void recordResult(int, char, bool, queue<GameResult>&);
void displayStatistics(queue<GameResult>&);
void printWelcome();
void printGameOver();

/************************************************************
* FUNCTION: main
*____________________________________________________________
* PURPOSE:
*    The entry point for the Mastermind game. Initializes 
*    game components, sets up the gameplay loop, processes 
*    player input, and manages game flow, including player 
*    statistics and exit conditions.
*
* LOCAL VARIABLES:
*    - queue<GameResult> resultsQueue: Holds results of each 
*                                      completed game for 
 *                                     later display.
*    - char playAgain: Indicates if the player wants to play 
*                      another game ('y' or 'n').
*    - list<char> code: Stores the randomly generated game 
*                       code.
*    - list<char> guess: Stores the player's current guess.
*    - char choiceDuplicate: Tracks whether duplicates are 
*                            allowed in the code.
*    - int length: Represents the length of the code.
*    - stack<int> turns: Holds the number of turns (max 10) 
*                        for each game.
*    - string guess_input: Stores player's input for each 
*                          guess.
*    - bool quit: Flag to control game exit.
*    - bool endGame: Indicates if the current game is 
 *                   complete.
*    - bool skipTurn: Skips the turn loop if necessary.
************************************************************/ 
int main() 
{
    queue<GameResult> resultsQueue;
    char playAgain = 'y';    
    list<char> code;
    list<char> guess;
    char choiceDuplicate;
    int length;
    stack<int> turns;
    string guess_input;
    bool quit = false;  // Flag to control exit
    
    setupGame();    //Setting up the random function
    printWelcome();
    
    do {
        bool endGame = false;
        bool skipTurn = false; // Flag to skip the turn without using `continue`
        playAgain = tolower(playAgain);
        
        if(playAgain == 'y') {
            for (int i = 1; i <= 10; i++) turns.push(i);
            
            // Get valid code length
            length = getCodeLength();

            // Get valid choice for duplicates
            choiceDuplicate = getDuplicateChoice();

            genCode(length, code, choiceDuplicate);
            //cout << "\t\tCODE: ";
            //printCode(code);
            cout << "\nWrite a code using the numbers from 1 to 8. You have 10 "
                    "turns to guess the code.\n";

            while (!endGame && !turns.empty() && !quit) {    
                skipTurn = false; // Reset skipTurn flag at the start of each turn
                cout << "Type 'exit' anytime to quit the game." << endl;
                cout << "Type 'tutorial' to see game's instructions." << endl;
                cout << "\nGuess: ";
                cin >> guess_input;

                // Check for exit command
                if (guess_input == "exit") {
                    exitingGame(quit);
                    skipTurn = true; // Skip rest of the loop for re-confirmation
                }
                
                if (guess_input == "tutorial") {
                    showInstructions();
                    skipTurn = true;
                }

                // First try-catch block: Check guess input
                if(!skipTurn){
                    validInput(guess_input, skipTurn, length);
                }

                // Clear the previous guess and add the new one from input
                if(!skipTurn){
                    compareGuess(guess, guess_input, code, endGame, turns, 
                                 length, choiceDuplicate, resultsQueue);
                }
            }

            if (!quit) {
                showGameOverMessage(code);
                displayStatistics(resultsQueue);  // Show statistics after each game
                newGame(playAgain);
            }
        }
        code.clear();
    } while (playAgain == 'y' && !quit);

    return 0;
}

/************************************************************
* FUNCTION: genCode
*____________________________________________________________
* PURPOSE:
*    Generates a random code for the Mastermind game based on
*    the specified length and duplicate setting.
*
* PARAMETERS:
*    - int length: Desired length of the generated code.
*    - list<char>& code: Reference to a list where the 
*      generated code will be stored.
*    - char choice: Character that indicates if duplicates 
*      are allowed ('Y' for yes, 'N' for no).
*
* LOCAL VARIABLES:
*    - set<char> unique_numbers: Used to store unique 
*      characters for codes with no duplicates.
*    - deque<char> numbers: Contains possible characters 
*      ('1' to '8') for code generation.
*    - auto num_int: Iterator pointing to the start of 
*      the numbers deque.
*____________________________________________________________ 
* RETURN: 
*    Void: Modifies the 'code' list to hold the newly
*    generated random sequence of characters based on 
*    the specified length and duplicate settings.   
************************************************************/ 
void genCode(int length, list<char>& code, char choice) {
    set<char> unique_numbers;
    deque<char> numbers = {'1', '2', '3', '4', '5', '6', '7', '8'};
    
    auto num_int = numbers.begin();
    
    while (code.size() < length) {
        random_shuffle(numbers.begin(), numbers.end());
        //char num = '1' + rand() % 8;
        if (toupper(choice) == 'Y' || unique_numbers.find(*num_int) == 
            unique_numbers.end()) {
            code.push_back(*num_int);
            unique_numbers.insert(*num_int);
            num_int++;
        }
    }
}

/************************************************************
* FUNCTION: printCode
*____________________________________________________________
* PURPOSE:
*    Displays the generated code sequence by printing each 
*    character in the code list.
*
* PARAMETERS:
*    - const list<char>& code: Reference to a list containing 
*      the code characters to be printed.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the code sequence to the console.  
************************************************************/ 
void printCode(const list<char>& code) {
    for (char num : code) {
        cout << num;
    }
    cout << endl;
}

/************************************************************
* FUNCTION: hint
*____________________________________________________________
* PURPOSE:
*    Generates a hint to guide the player by indicating 
*    the number of correct and misplaced digits in the guess.
*
* PARAMETERS:
*    - const list<char>& code: The actual code sequence the 
*      player is attempting to guess.
*    - const list<char>& guess: The player's current guess of
*      the code.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the hint string to the console, guiding 
*          the player in future guesses.
************************************************************/
void hint(const list<char>& code, const list<char>& guess) {
    map<char, int> code_count;
    int correct = 0;     // Counts correct positions (O's)
    int misplaced = 0;   // Counts misplaced digits (X's)
    
    // First pass: Check for correct positions and track remaining code digits
    auto code_it = code.begin();
    auto guess_it = guess.begin();
    while (code_it != code.end()) {
        if (*code_it == *guess_it) {
            correct++;  // Increment correct count for each matching position
        } else {
            code_count[*code_it]++;  // Track unmatched code digits for misplaced checking
        }
        ++code_it;
        ++guess_it;
    }

    // Second pass: Check for misplaced digits (wrong position but correct digit)
    guess_it = guess.begin();
    code_it = code.begin();
    while (guess_it != guess.end()) {
        auto found = find(code.begin(), code.end(), *guess_it); // Search for the guess in the rest of the code
        // Use find to check if *guess_it is in the remaining code characters
        if (*code_it != *guess_it && code_count[*found] > 0) {
                misplaced++;               // Increment misplaced count if digit is in the wrong position
                code_count[*found]--;   // Decrement count to avoid double-counting
            }
        ++guess_it;
        ++code_it;
    }
    
    // Create the hint result string
    string hint_result(correct, 'O');   // Add all 'O's for correct positions
    hint_result += string(misplaced, 'X'); // Add all 'X's for misplaced digits
    hint_result += string(code.size() - correct - misplaced, '_'); // Add all '_'s for incorrect digits

    // Print hint result
    cout << "Hint: " << hint_result << endl;
}

/************************************************************
* FUNCTION: setupGame
*____________________________________________________________
* PURPOSE:
*    Initializes the random number generator with the current 
*    time to ensure different random sequences in each game.
*____________________________________________________________
* RETURNS:
*    Void: Prepares randomization for generating elements.
************************************************************/
void setupGame(){
    srand(static_cast<unsigned int>(time(0)));
}

/************************************************************
* FUNCTION: getCodeLength
*____________________________________________________________
* PURPOSE:
*    Prompts the user to select a code length for the game 
*    and validates the input, ensuring it is 4, 6, or 8.
*____________________________________________________________
* RETURNS:
*    int: The validated code length chosen by the user.
************************************************************/
int getCodeLength(){
    int length;
    
    do {
        try {
            cout << "Choose the code length: " << endl;
            cout << "4" << endl;
            cout << "6" << endl;
            cout << "8" << endl;
            cin >> length;
            if (cin.fail()){ 
                throw invalid_argument("Invalid input type. Please enter a number.");
            }
            if (length != 4 && length != 6 && length != 8){ 
                throw invalid_argument("Invalid code length. Please enter 4, 6, or 8.");
            }
        } 
        catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(100, '\n');
            length = 0;
        }
    } while (length != 4 && length != 6 && length != 8);
    
    return length;    
}

/************************************************************
* FUNCTION: getDuplicateChoice
*____________________________________________________________
* PURPOSE:
*    Prompts the user to decide if duplicates are allowed in 
*    the game code, validating the input as either 'y' or 
*    'n'.
*____________________________________________________________
* RETURNS:
*    char: The user's validated choice for duplicates ('y' or
*          'n').
************************************************************/
char getDuplicateChoice(){
    char choiceDuplicate;
    
    do {
        try {
            cout << "Do you want to play with duplicates? [y/n]: ";
            cin >> choiceDuplicate;
            if (cin.fail()){ 
                throw invalid_argument("Invalid input type. Please enter 'y' or 'n'.");
            }
            choiceDuplicate = tolower(choiceDuplicate);
            if (choiceDuplicate != 'y' && choiceDuplicate != 'n'){ 
                throw invalid_argument("Invalid choice. Please enter 'y' or 'n'.");
            }
        } 
        catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(100, '\n');
            choiceDuplicate = '\0';
        }
    } while (choiceDuplicate != 'y' && choiceDuplicate != 'n');
    
    return choiceDuplicate;
}

/************************************************************
* FUNCTION: showGameOverMessage
*____________________________________________________________
* PURPOSE:
*    Displays the end-of-game message, reveals the correct 
*    code, and displays a game over message.
*
* PARAMETERS:
*    - const list<char> &code: The actual code used in the 
*                              game, which is revealed to the 
*                              player upon game over.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the code and game-over message.
************************************************************/
void showGameOverMessage(const list<char> &code){
    cout << "\nThe code was: ";
    printCode(code);
    printGameOver(); 
}

/************************************************************
* FUNCTION: newGame
*____________________________________________________________
* PURPOSE:
*    Ask the user if he or she wants to play again. If the
*    the user responds 'y', the game will start over with a
*    new secret code. If 'n', the program ends. 
*
* PARAMETERS:
*    - char &playAgain: User's choice ('y' or 'n')
*____________________________________________________________
* RETURNS:
*    Void: Outputs a message asking for a n0w game.
************************************************************/
void newGame(char &playAgain){
    cout << "Do you want to play again? [y/n]: ";
    cin >> playAgain;
    playAgain = tolower(playAgain);
    if (playAgain == 'n') {
        cout << "Thanks for playing! Goodbye!" << endl;
    }
}

/************************************************************
* FUNCTION: showInstructions
*____________________________________________________________
* PURPOSE:
*    Displays the instructions for the Mastermind game, 
*    explaining the rules, the goal of the game, how guesses 
*    and hints work, and how to enter valid inputs.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the game's instructions to the console.
************************************************************/
void showInstructions(){
    for(int i = 0 ; i < 80; i++){
        cout << "*";
    }
    
    cout << endl;
    cout << "*\t\t\tThis is Mastermind!" << endl << "*" << endl;
    cout << "*\tThe goal of the game is to guess the code the computer generated.";
    cout << endl << "*" << endl;
    cout << "*\tYou have 10 attempts to guess the code." << endl;
    cout << "*\tIn order to enter your guess, please type numbers from 1 to 8, "
            "\n*\taccording to the code size you selected (4, 6 or 8 digits).";
    cout << endl << "*" << endl;
    cout << "*\tFor every guess you entered, you will be given a hint in the form:";
    cout << endl << "*" << endl;
    cout << "*\tOOX_" << endl << "*" << endl;
    cout << "*\tThe symbols above represents the amount of digits in the right "
            "\n*\tposition, wrong position and incorrect digits from your guess: ";
    cout << endl;
    cout << "*\tO: One digit in the right position." << endl;
    cout << "*\tX: One digit in the wrong position. " << endl;
    cout << "*\t_: One incorrect digit." << endl << "*" << endl;    
    cout << "*\tFor instance, if the secret code is '1234' and your guess was "
            "\n*\t'5247', the hint will be OX__, because '2' was in the right "
            "\n*\tposition, '4' was in the wrong position and '5' and '7' were "
            "\n*\tincorrect digits. As you can notice, the hint does not show you "
            "\n*\twhat digit's place was right, wrong or incorrect, it only shows "
            "\n*\tthe amount." << endl << "*" << endl;
    cout << "*\t\t\tHAPPY GUESSING! :D" << endl;
    
    for(int i = 0 ; i < 80; i++){
        cout << "*";
    }
    cout << endl;
}

/************************************************************
* FUNCTION: validInput
*____________________________________________________________
* PURPOSE:
*    Validates the player's guess input for correctness 
*    in terms of format, length, and valid characters (1-8).
*
* PARAMETERS:
*    - const string& guess_input: The player's guess input to 
*                                 validate.
*    - bool& skipTurn: A flag that, when set to true, skips 
*                      the current turn if the input is 
*                      invalid.
*    - const int& length: The expected length of the guess.
*____________________________________________________________
* RETURNS:
*    Void: Outputs an error message and skips the turn if the 
*          input is invalid.
************************************************************/
void validInput(const string &guess_input, bool &skipTurn, const int &length){
    try {
        if (guess_input.empty()){ 
            throw invalid_argument("Input cannot be empty. Please try again.");
        }
        if (!all_of(guess_input.begin(), guess_input.end(), ::isdigit)){ 
            throw invalid_argument("Guess contains invalid characters. Use only numbers.");
        }
        if (guess_input.size() != length){ 
            throw invalid_argument("Guess length does not match the code length.");
        }
        for (char ch : guess_input) {
            if (ch < '1' || ch > '8') 
                throw invalid_argument("Guess contains invalid numbers. Only use 1 to 8.");
        }
    } catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
        skipTurn = true; // Skip turn if an invalid guess was made
    }
}

/************************************************************
* FUNCTION: compareGuess
*____________________________________________________________
* PURPOSE:
*    Compares the player's guess to the generated code, 
*    provides feedback through hints, and determines if the 
*    game is won or lost.
*
* PARAMETERS:
*    - list<char>& guess: The list storing the player's 
*                         current guess.
*    - const string& guess_input: The string containing the 
*                                 player's guess.
*    - const list<char>& code: The list storing the generated 
*                              code.
*    - bool& endGame: A flag that indicates if the game has 
*                     ended.
*    - stack<int>& turns: A stack holding the remaining 
*                         turns.
*    - const int& length: The length of the code/guess.
*    - const char& choiceDuplicate: A character indicating 
*                                   whether duplicates are 
*                                   allowed.
*    - queue<GameResult>& resultsQueue: A queue to store game 
*                                       results.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the result of the guess, updates the game 
*          status, and records the game result.
************************************************************/
void compareGuess(list<char>& guess, const string& guess_input, 
                  const list<char>& code, bool& endGame, stack<int>& turns,
                  const int &length, const char &choiceDuplicate,
                  queue<GameResult>& resultsQueue){
    guess.clear();
    for (char ch : guess_input){ 
        guess.push_back(ch);
    }

    if (code == guess) {
        endGame = true;
        recordResult(length, choiceDuplicate, true, resultsQueue); // Record win
        cout << "Congratulations!! You win !!" << endl; 
        while(!turns.empty()){
            turns.pop();
        }
    } else {
        hint(code, guess);
        if (!turns.empty()) {
            turns.pop();
            cout << "Turns left: " << (turns.empty() ? 0 : turns.top()) << endl;
            if(turns.empty()){
                //cout << "\t\tTURNS ARE EMPTY" << endl;
                recordResult(length, choiceDuplicate, false, resultsQueue); // Record loss
            }
        }

    }
}

/************************************************************
* FUNCTION: exitingGame
*____________________________________________________________
* PURPOSE:
*    Asks the player for confirmation to exit the game and 
*    sets the quit flag if the player confirms.
*
* PARAMETERS:
*    - bool& quit: A reference to a flag that controls if the 
*                  game loop continues or exits.
*____________________________________________________________
* RETURNS:
*    Void: Exits the game if the player confirms, otherwise 
 *         continues.
************************************************************/
void  exitingGame(bool &quit){
    char confirm;
    cout << "Are you sure you want to quit? [y/n]: ";
    cin >> confirm;
    if (tolower(confirm) == 'y') {
        cout << "Exiting game. Thanks for playing!" << endl;
        quit = true; // Set quit flag to exit loop
    }    
}

/************************************************************
* FUNCTION: recordResult
*____________________________________________________________
* PURPOSE:
*    Records the outcome of a single game (win/loss) and its 
*    settings into a queue for tracking game results.
*
* PARAMETERS:
*    - int codeLength: The code's length used in the game.
*    - char duplicateSetting: The setting for duplicates ('y' 
*                             or 'n').
*    - bool isWin: True if user won the game, false if lost.
*    - queue<GameResult>& resultsQueue: Reference to queue 
*                                       where game results
*                                       are stored.
*____________________________________________________________
* RETURNS:
*    Void: Adds the game result to the resultsQueue.
************************************************************/
void recordResult(int codeLength, char duplicateSetting, bool isWin, 
                  queue<GameResult> &resultsQueue) {
    resultsQueue.push(GameResult(codeLength, duplicateSetting, isWin));
}

/************************************************************
* FUNCTION: displayStatistics
*____________________________________________________________
* PURPOSE:
*    Displays the statistics of the game results, including 
*    wins and losses for different code lengths (4, 6, 8) and 
*    settings for duplicates, and compares the number of wins 
*    with and without duplicates.
*
* PARAMETERS:
*    - queue<GameResult>& resultsQueue: A reference to the 
*                                       queue containing the
*                                       results of previous 
*                                       games to be analyzed.
*____________________________________________________________
* RETURNS:
*    Void: Outputs game statistics to the console.
************************************************************/
void displayStatistics(queue<GameResult> &resultsQueue) {
    static int wins_4[2] = {0, 0}, losses_4[2] = {0, 0};
    static int wins_6[2] = {0, 0}, losses_6[2] = {0, 0};
    static int wins_8[2] = {0, 0}, losses_8[2] = {0, 0};

    // Process results in the queue
    while (!resultsQueue.empty()) {
        GameResult result = resultsQueue.front();
        resultsQueue.pop();

        int dupIndex = (result.duplicateSetting == 'y') ? 1 : 0;
        if (result.isWin) {
            if (result.codeLength == 4) wins_4[dupIndex]++;
            else if (result.codeLength == 6) wins_6[dupIndex]++;
            else if (result.codeLength == 8) wins_8[dupIndex]++;
        } else {
            if (result.codeLength == 4) losses_4[dupIndex]++;
            else if (result.codeLength == 6) losses_6[dupIndex]++;
            else if (result.codeLength == 8) losses_8[dupIndex]++;
        }
    }

    // Output results
    cout << "\nGame Statistics:\n";
    cout << "Code Length 4: Wins [No Dup: " << wins_4[0] << ", Dup: " << wins_4[1] << "], "
         << "Losses [No Dup: " << losses_4[0] << ", Dup: " << losses_4[1] << "]\n";
    cout << "Code Length 6: Wins [No Dup: " << wins_6[0] << ", Dup: " << wins_6[1] << "], "
         << "Losses [No Dup: " << losses_6[0] << ", Dup: " << losses_6[1] << "]\n";
    cout << "Code Length 8: Wins [No Dup: " << wins_8[0] << ", Dup: " << wins_8[1] << "], "
         << "Losses [No Dup: " << losses_8[0] << ", Dup: " << losses_8[1] << "]\n";
    
    // Use max_element and min_element to find the most and least wins for duplicates vs no-duplicates
    int wins[] = {wins_4[0] + wins_6[0] + wins_8[0], wins_4[1] + wins_6[1] + wins_8[1]};
    int* maxWins = max_element(wins, wins + 2);
    int* minWins = min_element(wins, wins + 2);
    
    if(wins[0] != wins[1]){
        cout << "\nMore victories: ";
        if (*maxWins == wins[0]) {
            cout << "No Duplicates (" << *maxWins << " wins)" << endl;
        } else {
            cout << "With Duplicates (" << *maxWins << " wins)" << endl;
        }

        cout << "Fewer victories: ";
        if (*minWins == wins[0]) {
            cout << "No Duplicates (" << *minWins << " wins)" << endl;
        } else {
            cout << "With Duplicates (" << *minWins << " wins)" << endl;
        }
    }
}

/************************************************************
* FUNCTION: printWelcome
*____________________________________________________________
* PURPOSE:
*    Create a visually striking title screen for the game.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the game's title to the console.
************************************************************/
void printWelcome(){
    cout << endl;
    cout << "              (           )    *                    )      *      "
            "     (             (      *    (       ) (      " << endl;
    cout << " (  (         )\\ )  (  ( /(  (  `          *   ) ( /(    (  `    ("
            "     )\\ ) *   )    )\\ ) (  `   )\\ ) ( /( )\\ )   " << endl;
    cout << " )\\))(   '(  (()/(  )\\ )\\()) )\\))(  (    ` )  /( )\\())   )\\))(   )"
            "\\   (()/` )  /((  (()/( )\\))( (()/( )\\()(()/(   " << endl;
    cout << "((_)()\\ ) )\\  /(_)(((_((_)\\ ((_)()\\ )\\    ( )(_)((_)\\   ((_)()(((("
            "_)(  /(_)( )(_))\\  /(_)((_)()\\ /(_)((_)\\ /(_))  " << endl;
    cout << "_(())\\_)(((_)(_)) )\\___ ((_)(_()((_((_)  (_(_())  ((_)  (_()((_)\\ "
            "_ )\\(_))(_(_()((_)(_)) (_()((_(_))  _((_(_))_   " << endl;
    cout << "\\ \\((_)/ | __| | ((/ __/ _ \\|  \\/  | __| |_   _| / _ \\  |  \\/  (_)"
            "_\\(_/ __|_   _| __| _ \\|  \\/  |_ _|| \\| ||   \\  " << endl;
    cout << " \\ \\/\\/ /| _|| |__| (_| (_) | |\\/| | _|    | |  | (_) | | |\\/| |/ "
            "_ \\ \\__ \\ | | | _||   /| |\\/| || | | .` || |) | " << endl;
    cout << "  \\_/\\_/ |___|____|\\___\\___/|_|  |_|___|   |_|   \\___/  |_|  |_/_/"
            " \\_\\|___/ |_| |___|_|_\\|_|  |_|___||_|\\_||___/  " << endl << endl;    
}

/************************************************************
* FUNCTION: printGameOver
*____________________________________________________________
* PURPOSE:
*    Create a ASCII art-style game over message.
*____________________________________________________________
* RETURNS:
*    Void: Outputs the game's game over message.
************************************************************/
void printGameOver(){
    cout << endl;
    cout << "  #####     #    #     # #######       #######  #     # ####### ######  " << endl;
    cout << " #     #   # #   ##   ## #             #     #  #     # #       #     # " << endl;
    cout << " #        #   #  # # # # #             #     #  #     # #       #     # " << endl;
    cout << " #  #### #     # #  #  # #####         #     #  #     # #####   ######  " << endl;
    cout << " #     # ####### #     # #             #     #   #   #  #       #   #   " << endl;
    cout << " #     # #     # #     # #             #     #    # #   #       #    #  " << endl;
    cout << "  #####  #     # #     # #######       #######     #    ####### #     # " << endl;
    cout << endl;
}
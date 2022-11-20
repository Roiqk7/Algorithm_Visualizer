// * g++ -std=c++17 test.cpp -o test -I/Users/roiqk/SFML/include -L/Users/roiqk/SFML/build/lib  -lsfml-graphics -lsfml-window -lsfml-system



//  this program visualizes backtracking algorithm solving N Queens problem

#include <array>        
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>

//  map related -> base N = 8; TILE_SIZE = 100 
//  eg if you want to increase N from 8 to 16 you have to decrease TILE_SIZE from 100 to 50
#define N 8                    //  size of one side of the square map
#define TILE_SIZE 100          //  size of one tile on the map

//  window related
#define WIDTH N*TILE_SIZE    //  window width
#define HEIGHT N*TILE_SIZE   //  window height

// return values
#define SUCCESS 0
#define ERROR -1

// create the window     
sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIDTH, HEIGHT)), "Backtracking algorithm visualizer");

//  constant values describing board
const int QUEEN_NUM = 1;
const int EMPTY_NUM = 0;


/*
     ######  ##          ###     ######   ######  
    ##    ## ##         ## ##   ##    ## ##    ## 
    ##       ##        ##   ##  ##       ##       
    ##       ##       ##     ##  ######   ######  
    ##       ##       #########       ##       ## 
    ##    ## ##       ##     ## ##    ## ##    ## 
     ######  ######## ##     ##  ######   ######  
*/


//  Board stores all important data and renders the actual board seen by the user
class Board {
    public:
        std::array<sf::RectangleShape, N> boardTiles;
        std::array<std::array<int, N>, N> map;
        int black;        

        Board() {
            map = {{EMPTY_NUM}};
            black = 0;
        }

        //  draws the bard
        void render(void)
        {
            window.clear();
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    boardTiles[i].setPosition (sf::Vector2f(j*TILE_SIZE, i*TILE_SIZE));
                    boardTiles[i].setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                    if (map[i][j] == QUEEN_NUM) boardTiles[i].setFillColor(sf::Color::Green);
                    else if (black % 2 == true) boardTiles[i].setFillColor(sf::Color::Black);
                    else boardTiles[i].setFillColor(sf::Color::White);
                    window.draw(boardTiles[i]);
                    black++; 
                }
                black++;
            }
            window.display();
        }
};


void addRandomQueen(Board &board);              //  adds random queen for uniqueness
void sfml(Board &board);                        //  draws the screen
bool NQsolve(Board &board, int row);            //  backtracking algorithm solving N Queen
void printOut(Board &board);                    //  prints board to the console


/*
    ##     ##    ###    #### ##    ## 
    ###   ###   ## ##    ##  ###   ## 
    #### ####  ##   ##   ##  ####  ## 
    ## ### ## ##     ##  ##  ## ## ## 
    ##     ## #########  ##  ##  #### 
    ##     ## ##     ##  ##  ##   ### 
    ##     ## ##     ## #### ##    ## 
*/


//  starts the program
int main()
{
    while (true) {
        Board board = Board();

        addRandomQueen(board);

        sfml(board);

    }
    return SUCCESS;
}


//  makes the program more unique and interesting
void addRandomQueen(Board &board)
{
    srand(time(NULL));
    int row = rand()%N;
    int col = rand()%N;
    board.map[row][col] = QUEEN_NUM;
}


/*
     ######  ######## ##     ## ##       
    ##    ## ##       ###   ### ##       
    ##       ##       #### #### ##       
     ######  ######   ## ### ## ##       
          ## ##       ##     ## ##       
    ##    ## ##       ##     ## ##       
     ######  ##       ##     ## ######## 
*/


//  visualizes the algorithm
void sfml(Board &board)
{
    window.setFramerateLimit (60);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        NQsolve(board, 0);
    }
    return;
}


/*
       ###    ##        ######    #######  ########  #### ######## ##     ## ##     ## 
      ## ##   ##       ##    ##  ##     ## ##     ##  ##     ##    ##     ## ###   ###
     ##   ##  ##       ##        ##     ## ##     ##  ##     ##    ##     ## #### #### 
    ##     ## ##       ##   #### ##     ## ########   ##     ##    ######### ## ### ## 
    ######### ##       ##    ##  ##     ## ##   ##    ##     ##    ##     ## ##     ##     
    ##     ## ##       ##    ##  ##     ## ##    ##   ##     ##    ##     ## ##     ##
    ##     ## ########  ######    #######  ##     ## ####    ##    ##     ## ##     ##  
*/


//  checks if queen can be placed on specific position on the board
bool isValid(Board &board, int &row, int &col)
{
    for (int i = 0; i < N; i++) if (board.map[i][col] == QUEEN_NUM) return false;
    for (int i = 0; i < N; i++) { 
        for (int j = 0; j < N; j++) {
            if (i + j == row + col || i - j == row - col) {
                if (board.map[i][j] == QUEEN_NUM) return false;
            }
        }
    }
    return true;
}


/*
* Implementation of backtracking algorithm, O(n!)
The idea is to place queens one by one in different columns, 
starting from the leftmost column. When we place a queen in a column, 
we check for clashes with already placed queens. In the current column, 
if we find a row for which there is no clash, we mark this row and column as part of the solution. 
If we do not find such a row due to clashes, then we backtrack and return false.
*/
bool NQsolve(Board &board, int row) 
{
    if (row == N) return true;
    for (int i = 0; i < N; i++) if (board.map[row][i] == QUEEN_NUM) return NQsolve(board, row+1);
    for (int i = 0; i < N; i++) {
        if (isValid(board, row, i)) {
            board.map[row][i] = QUEEN_NUM;
            board.render();
            printOut(board);
            if (NQsolve(board, row+1)) return true;
        }
        board.map[row][i] = EMPTY_NUM;
    }
    return false;
}


//  print the board in user-readable form and returns Queen positions (not designed for N > 27)
void printOut(Board &board)
{
    std::cout << std::endl;
    std::string formattedReturn;
    for (int i = 0; i < N; i++) std::cout << ' ' << (char) (97 + i);
    std::cout << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board.map[i][j] == QUEEN_NUM) {
                std::cout << ' ' << 'Q';
                formattedReturn += 'a' + i%26;
                formattedReturn += std::to_string(N - j);
                if (i != N - 1 && j != N - 1) formattedReturn += ", ";
            }
            else std::cout << ' ' << '.';
        }
        std::cout << '|' << N - i << '\n';
    }
    std::cout << '\n' << formattedReturn << '\n';
}

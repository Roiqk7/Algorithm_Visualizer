//  TODO mergesort 
// *make (improve) current element which is being examined green


//  g++ -std=c++17 test.cpp -o test -I/Users/roiqk/SFML/include -L/Users/roiqk/SFML/build/lib  -lsfml-graphics -lsfml-window -lsfml-system


//  this program visualizes sorting algorithms using SFML
//  * https://www.sfml-dev.org/index.php

#include <array>        
#include <chrono>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <unistd.h> 

//  array related
using std::array;                       //  make array from std visible
#define SIZE 150                        //  size of array to be sorted
#define COL_WIDTH 10                    //  width of col representing number in an array
#define MAX 200                         //  maximum value for random number inside sortMe array
#define COL_MULTIPLIER 2.5              //  Multiplies cols for better visuals
#define WIDTH SIZE*COL_WIDTH            //  Height of the window
#define HEIGHT MAX*COL_MULTIPLIER+50    //  Width of the window

//  time 
#define MICRO_TO_MIL 1000

// return values
#define SUCCESS 0
#define ERROR -1

enum SortAlgorithm {bubble = 1, selection = 2, merge = 3};

/*
     ######  ##          ###     ######   ######  
    ##    ## ##         ## ##   ##    ## ##    ## 
    ##       ##        ##   ##  ##       ##       
    ##       ##       ##     ##  ######   ######  
    ##       ##       #########       ##       ## 
    ##    ## ##       ##     ## ##    ## ##    ## 
     ######  ######## ##     ##  ######   ######  
*/

//  class for the array which holds values
class SortMe {
    public:
        array<int, SIZE> arr;
        SortMe() {
            this->arr = generateUnsortedArray();
        }

        //  returns arr[i] if user types sortMe[i]
        int &operator[](int i){return arr[i];}

        //  draws the array on screen
        array<sf::RectangleShape, SIZE> render(array<int, SIZE> &arr, int &currentCol)
        {
            array<sf::RectangleShape, SIZE> arrOfRects;
            for (int i = 0; i < SIZE; i++) {
                arrOfRects[i].setSize(sf::Vector2f(COL_WIDTH, arr[i]*COL_MULTIPLIER));
                if (i == currentCol) arrOfRects[currentCol].setFillColor(sf::Color::Green);
                else arrOfRects[i].setFillColor(sf::Color::White);
                arrOfRects[i].setPosition (sf::Vector2f(i*COL_WIDTH, HEIGHT-arr[i]*COL_MULTIPLIER));
            }
            return arrOfRects;
        }

    private:
        // makes sure elements dont repeat in the array 
        bool isNotInArr(array<int, SIZE> &arr, int num)
        {
            for (int i = 0; i < SIZE; i++) if (arr[i] == num) return false;
            return true;
        }

        //  creates unsorted random array of size SIZE
        array<int, SIZE> generateUnsortedArray(void) {
            srand(time(NULL));
            array<int, SIZE> arr;
            for (int i = 0; i != SIZE;) {
                int num = rand()%MAX;
                if (isNotInArr(arr, num)) {
                    arr[i] = num;
                    i++;
                }
                continue;
            }
            return arr;
        }       
};


void stopwatch(void);                                               //  times execution time
int sfml(SortMe &sortMe, int &selectedAlgorithm, long &speed);      //  sfml gui
void bubbleSort(SortMe &sortMe, int &currentCol);                   //  implements bubble sort 
void selectionSort(SortMe &sortMe, int &currentCol);                //  implements selection sort
void printOut(SortMe &sortMe);                                      //  prints sorted array

/*
    ##     ##    ###    #### ##    ## 
    ###   ###   ## ##    ##  ###   ## 
    #### ####  ##   ##   ##  ####  ## 
    ## ### ## ##     ##  ##  ## ## ## 
    ##     ## #########  ##  ##  #### 
    ##     ## ##     ##  ##  ##   ### 
    ##     ## ##     ## #### ##    ## 
*/


//  main() serves the purpose of getting valid user input and kicking off the whole program
int main(void)
{
    int selectedAlgorithm;      //  gets valid user input
    long speed;
    while (true) {
        do {
            std::cout << "Press (" << bubble << ") for bubble sort algorithm\n";
            std::cout << "Press (" << selection << ") for selection sort algorithm\n";
            std::cout << "Press (" << merge << ") for merge sort algorithm\n";
            std::cout << "Enter number of the algorithm you wish to visualize: ";
            std::cin >> selectedAlgorithm;
            std::cout << "Enter speed in milliseconds: ";
            std::cin >> speed;
        }
        while (selectedAlgorithm < 0 || speed < 0);

        //  initiates SortMe object
        SortMe sortMe = SortMe();                                       

        //  once user entered valid input, timer starts
        stopwatch();                                                    

        //  starts drawing the window 
        sfml(sortMe, selectedAlgorithm, speed);                         
    }
    return SUCCESS;
}


//  times execution time using coroutines
void stopwatch(void)
{    
    static int state = 0;
    static std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    if (state == 0) {
        state = 1;
        start = std::chrono::high_resolution_clock::now();  
        return;
    }
    state = 0;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "\nFinished in " << duration.count() << " microseconds" << std::endl;
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
int sfml(SortMe &sortMe, int &selectedAlgorithm, long &speed)
{
    // create the window     
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIDTH, HEIGHT)), "Sort Visualizer");
    window.setFramerateLimit (60);

    //  run the program as long as the window is open
    int currentCol = 0;
    while (window.isOpen())
    {
        if (currentCol == SIZE) stopwatch();
        usleep(speed * MICRO_TO_MIL);
        switch (selectedAlgorithm) {     

            case bubble:                                    
                bubbleSort(sortMe, currentCol);
                break;
            case selection: 
                selectionSort(sortMe, currentCol);
                break;
            case merge:  
                // TODO
                break;
        }

        //  check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            //  "close requested" event: we close the window
            if (event.type == sf::Event::Closed) window.close();
        }

        //  clear the window with black color
        window.clear(sf::Color::Black);

        //  draw the cols
        for (int i = 0; i < SIZE; i++) window.draw(sortMe.render(sortMe.arr, currentCol)[i]);

        currentCol++;

        //  end the current frame
        window.display();
    }

    return SUCCESS;
}


/*
       ###    ##        ######    #######  ########  #### ######## ##     ## ##     ##  ######  
      ## ##   ##       ##    ##  ##     ## ##     ##  ##     ##    ##     ## ###   ### ##    ## 
     ##   ##  ##       ##        ##     ## ##     ##  ##     ##    ##     ## #### #### ##       
    ##     ## ##       ##   #### ##     ## ########   ##     ##    ######### ## ### ##  ######  
    ######### ##       ##    ##  ##     ## ##   ##    ##     ##    ##     ## ##     ##       ## 
    ##     ## ##       ##    ##  ##     ## ##    ##   ##     ##    ##     ## ##     ## ##    ## 
    ##     ## ########  ######    #######  ##     ## ####    ##    ##     ## ##     ##  ######  
*/


//  implementation of bubble sort
//  time complexity: O(n^2)
void bubbleSort(SortMe &sortMe, int &currentCol)
{
    bool unsorted;
    do {
        unsorted = false;
        for (int i = 0; i < (currentCol - 1); i++) {
            if (sortMe.arr[i] > sortMe.arr[i + 1]) {
                unsorted = true;
                for (; i < (currentCol - 1); i++) {
                    if (sortMe.arr[i] > sortMe.arr[i + 1]) {
                        std::swap(sortMe.arr[i], sortMe.arr[i + 1]);
                    }
                }
            }
        }
    } while (unsorted);
}


//  implementation of selection sort
//  time complexity: O(n^2)
void selectionSort(SortMe &sortMe, int &currentCol)
{
    int minIndex;
    if (currentCol != SIZE) {
        for (int i = minIndex = currentCol; i < SIZE; i++) {          
            if (sortMe[minIndex] > sortMe[i]) minIndex = i;      
        }
        std::swap(sortMe.arr[minIndex], sortMe.arr[currentCol]);
        return;
    } 
}

void mergeSort(void)
{
    //  TODO
    return;
}

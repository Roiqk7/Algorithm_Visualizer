//  ! mergesort doesn't show up -> in progress

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
#define SIZE 300                        //  size of array to be sorted
#define COL_WIDTH 5                     //  width of col representing number in an array
#define COL_MULTIPLIER 2                //  Multiplies cols for better visuals
#define WIDTH SIZE*COL_WIDTH            //  Width of the window
#define HEIGHT SIZE*COL_MULTIPLIER      //  Height of the window

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
        int changedIndex;
        int currentCol;

        SortMe() {
            arr = generateUnsortedArray();
            changedIndex = 0;
            currentCol = 0;
        }

        //  returns arr[i] if user types sortMe[i]
        int &operator[](int i){return arr[i];}

        //  draws the array on screen
        array<sf::RectangleShape, SIZE> render(SortMe &sortMe)
        {
            array<sf::RectangleShape, SIZE> arrOfRects;
            for (int i = 0; i < SIZE; i++) {
                arrOfRects[i].setPosition (sf::Vector2f(i*COL_WIDTH, HEIGHT-sortMe[i]*COL_MULTIPLIER));
                arrOfRects[i].setSize(sf::Vector2f(COL_WIDTH, sortMe[i]*COL_MULTIPLIER));
                //  colors current col and swapped col if array is unsorted
                if (currentCol < SIZE) {
                    if (i == currentCol) arrOfRects[currentCol].setFillColor(sf::Color::Green);
                    else arrOfRects[i].setFillColor(sf::Color::White);
                    //  colors changed col to deeper the understanding of the user about the algorithm
                    arrOfRects[changedIndex].setFillColor(sf::Color::Green);
                    //  places the cols
                }
                else {
                    arrOfRects[i].setFillColor(sf::Color::Green);
                }
            }
            return arrOfRects;
        }

    private:
        // makes sure elements dont repeat in the array 
        bool isNotInArr(array<int, SIZE> &arr, int num, int index)
        {
            for (int i = 0; i < index; i++) if (arr[i] == num) return false;
            return true;
        }

        //  creates unsorted random array of size SIZE
        array<int, SIZE> generateUnsortedArray(void) {
            srand(time(NULL));
            array<int, SIZE> arr;
            for (int i = 0; i < SIZE;) {
                int num = rand()%SIZE;
                if (isNotInArr(arr, num, i)) {
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
void bubbleSort(SortMe &sortMe);                                    //  implements bubble sort 
void selectionSort(SortMe &sortMe);                                 //  implements selection sort
void mergeSort(SortMe &sortMe, const int &start, const int &end);   //  implements merge sort


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
    int selectedAlgorithm;     
    long speed;
    while (true) {
        do {
            std::cout << "\n\nPress (" << bubble << ") for bubble sort algorithm\n";
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
    while (window.isOpen())
    {
        //  check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            //  "close requested" event: we close the window
            if (event.type == sf::Event::Closed) window.close();
        }

        //  if sorted stops refreshing the window and waits for user to close the window
        if (sortMe.currentCol == SIZE+1) continue;

        //  prints current col in the terminal
        std::cout << sortMe.currentCol << " is the current col.\n";

        //  if sorted then prints the runtime
        if (sortMe.currentCol == SIZE) stopwatch();

        //  clear the window with black color
        window.clear(sf::Color::Black);

        //  draw the cols
        for (int i = 0; i < SIZE; i++) window.draw(sortMe.render(sortMe)[i]);

        //  sort
        switch (selectedAlgorithm) {     
            case bubble:                                    
                bubbleSort(sortMe);
                break;
            case selection: 
                selectionSort(sortMe);
                break;
            case merge:  
                mergeSort(sortMe, 0, SIZE - 1);
                break;
        }

        //  makes sure to correctly increment current col value
        if (sortMe.currentCol < SIZE+1 && selectedAlgorithm != merge) sortMe.currentCol++;

        //  end the current frame
        window.display();
        usleep(speed * MICRO_TO_MIL);
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
void bubbleSort(SortMe &sortMe)
{
    bool unsorted;
    do {
        unsorted = false;
        for (int i = 0; i < (sortMe.currentCol - 1); i++) {
            if (sortMe.arr[i] > sortMe.arr[i + 1]) {
                unsorted = true;
                for (; i < (sortMe.currentCol - 1); i++) {
                    if (sortMe.arr[i] > sortMe.arr[i + 1]) {
                        std::swap(sortMe.arr[i], sortMe.arr[i + 1]);
                        sortMe.changedIndex = i + 1;
                    }
                }
            }
        }
    } while (unsorted);
}


//  implementation of selection sort
//  time complexity: O(n^2)
void selectionSort(SortMe &sortMe)
{
    int minIndex;
    if (sortMe.currentCol < SIZE) {
        for (int i = minIndex = sortMe.currentCol; i < SIZE; i++) {          
            if (sortMe[minIndex] > sortMe[i]) minIndex = i;      
        }
        std::swap(sortMe.arr[minIndex], sortMe.arr[sortMe.currentCol]);
        sortMe.changedIndex = minIndex;
    } 
}


//  merges the array
void mergeMe(SortMe &sortMe, const int &left, const int &mid, const int &right)
{
    const int leftSize = mid - left + 1;
    const int rightSize = right - mid;

    int *leftArr = new int[leftSize], *rightArr = new int[rightSize];

    //  copy into helper arrays
    for (int i = 0; i < leftSize; i++) leftArr[i] = sortMe[left + i], sortMe.currentCol = left + i;
    for (int i = 0; i < rightSize; i++) rightArr[i] = sortMe[mid + 1 + i], sortMe.currentCol = mid + 1 + i;

    int leftIndex, rightIndex;
    leftIndex = rightIndex = 0;
    sortMe.currentCol = left;

    //  copy into sortMe
    while (leftIndex < leftSize && rightIndex < rightSize) {
        if (leftArr[leftIndex] <= rightArr[rightIndex]) {
            sortMe[sortMe.currentCol] = leftArr[leftIndex];
            sortMe.changedIndex = leftIndex;
            leftIndex++;
        }
        else {
            sortMe[sortMe.currentCol] = rightArr[rightIndex];
            sortMe.changedIndex = rightIndex;
            rightIndex++;
        }
        sortMe.currentCol++;
    }

    //  copy the remaining values
    while (leftIndex < leftSize) {
        sortMe[sortMe.currentCol] = leftArr[leftIndex];
        sortMe.changedIndex = leftIndex;
        leftIndex++;
        sortMe.currentCol++;
    }

    while (rightIndex < rightSize) {
        sortMe[sortMe.currentCol] = rightArr[rightIndex];
        sortMe.changedIndex = rightIndex;
        rightIndex++;
        sortMe.currentCol++;
    }

    delete[] leftArr;
    delete[] rightArr;
}


//  implementation of merge sort (2/2)
//  time complexity: O(n log n)
void mergeSort(SortMe &sortMe, const int &start, const int &end)
{
    if (start >= end) return;
    int mid = start + (end - start) / 2;
    mergeSort(sortMe, start, mid);
    mergeSort(sortMe, mid + 1, end);
    mergeMe(sortMe, start, mid, end);
}

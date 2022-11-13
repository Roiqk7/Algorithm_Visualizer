/*
TODO - merge visualize -> while, if, static etc
*/


//  g++ -std=c++17 test.cpp -o test -I/Users/roiqk/SFML/include -L/Users/roiqk/SFML/build/lib  -lsfml-graphics -lsfml-window -lsfml-system


//  this program visualizes sorting algorithms using SFML
//  * https://www.sfml-dev.org/index.php

#include <array>        
#include <chrono>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <string.h>
#include <time.h>
#include <unistd.h> 

//  array related
#define SIZE 1500                       //  size of array to be sorted
#define COL_WIDTH 1                     //  width of col representing number in an array
#define COL_MULTIPLIER 0.4              //  multiplies col value
#define WIDTH SIZE*COL_WIDTH            //  width of the window
#define HEIGHT SIZE*COL_MULTIPLIER      //  height of the window

//  time 
#define MICRO_TO_MIL 1000

// return values
#define SUCCESS 0
#define ERROR -1

//  algorithm identification
#define sortAlgsCount 6
enum SortAlgsEnum {NONE, bubble, selection, insertion, cocktail, merge, heap};
const std::string sortAlgsNames[sortAlgsCount] = {"bubble", "selection", "insertion", "cocktail", "merge", "heap"};


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
        std::array<int, SIZE> arr;
        int currentCol;
        bool sorted;        
        int speed;
        int phase;          //  used for heap sort

        SortMe() {
            arr = generateUnsortedArray();
            currentCol = speed = 0;
            sorted = false;
            phase = 0;
        }

        //  returns arr[i] if user types sortMe[i]
        int &operator[](int i){return arr[i];}

        //  draws the array on screen
        std::array<sf::RectangleShape, SIZE> render()
        {
            std::array<sf::RectangleShape, SIZE> arrOfRects;
            for (int i = 0; i < SIZE; i++) {
                arrOfRects[i].setPosition (sf::Vector2f(i*COL_WIDTH, HEIGHT-arr[i]*COL_MULTIPLIER));
                arrOfRects[i].setSize(sf::Vector2f(COL_WIDTH, arr[i]*COL_MULTIPLIER));
                if (!sorted) {
                    if (i == currentCol ) arrOfRects[i].setFillColor(sf::Color::Green);
                    else arrOfRects[i].setFillColor(sf::Color::White);
                }
                else {
                    arrOfRects[i].setFillColor(sf::Color::Green);
                }
            }
            return arrOfRects;
        }

        //  executes all actions which need to be taken every round
        void doEveryRound()
        {
            isSorted();
            sleep();
        }

    private:
        // makes sure elements dont repeat in the array 
        bool isNotInArr(std::array<int, SIZE> &arr, int num, int index)
        {
            for (int i = 0; i < index; i++) if (arr[i] == num) return false;
            return true;
        }

        //  creates unsorted random array of size SIZE
        std::array<int, SIZE> generateUnsortedArray(void) {
            srand(time(NULL));
            std::array<int, SIZE> arr;
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

        //  linearly checks if array is sorted
        void isSorted()
        {
            for (int i = 1; i < SIZE; i++) if (arr[i - 1] >= arr[i]) {
                sorted = false; 
                return;
            }
            sorted = true;
        }

        //  slows down the program on desired speed
        void sleep()
        {
            usleep(speed * MICRO_TO_MIL);
        }  
};


//  Prototypes with quick orientation details

//  MAIN
void stopwatch(SortMe &sortMe);                                     //  times execution time
//  SFML
int sfml(SortMe &sortMe, int &selectedAlgorithm);                   //  sfml gui
//  ALGORITHMS
//  O(N^2)
void bubbleSort(SortMe &sortMe);                                    //  implements bubble sort 
void selectionSort(SortMe &sortMe);                                 //  implements selection sort
void insertionSort(SortMe &sortMe);                                 //  implementation of insertion sort
void cocktailSort(SortMe &sortMe);                                  //  implementation of cocktail sort
//  O(N LOG N)
void mergeSort(SortMe &sortMe, const int &start, const int &end);   //  implements merge sort
void heapSort(SortMe &sortMe);                                      //  implements heap sort
//  Note: see detailed description of the algorithms 


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
    while (true) {
        SortMe sortMe = SortMe();
        do {     
            //  ? update to for loop
            std::cout << "\n\n";  
            for (int i = 0; i < sortAlgsCount; i++) std::cout << "Press (" << i + 1 << ") for " << sortAlgsNames[i] << " sort algorithm\n";
            std::cout << "Enter number of the algorithm you wish to visualize: ";
            std::cin >> selectedAlgorithm;
            std::cout << "Enter speed in milliseconds: ";
            std::cin >> sortMe.speed;
        }
        while (selectedAlgorithm < 0 || sortMe.speed < 0);                                       

        //  once user entered valid input, timer starts
        stopwatch(sortMe);                                                    

        //  starts drawing the window 
        if (sfml(sortMe, selectedAlgorithm) == ERROR) return ERROR; 
    }
    return SUCCESS;
}


//  times execution time using coroutines and sets sorted status to prevent infinite loop
void stopwatch(SortMe &sortMe)
{    
    static int state = 0;
    static std::chrono::high_resolution_clock::time_point start;
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
int sfml(SortMe &sortMe, int &selectedAlgorithm)
{
    // create the window     
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIDTH, HEIGHT)), "Sort Visualizer");
    window.setFramerateLimit (60);

    //  run the program as long as the window is open
    while (window.isOpen()) {
        sortMe.doEveryRound();

        //  check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            //  handles closure of the window
            if (event.type == sf::Event::Closed) window.close();
        }

        //  clear the window with black color
        window.clear(sf::Color::Black);

        //  draw the cols
        auto arr = sortMe.render();
        for (int i = 0; i < SIZE; i++) window.draw(arr[i]);

        //  sort
        switch (selectedAlgorithm) {     
            case bubble:                                    
                bubbleSort(sortMe);
                break;
            case selection: 
                selectionSort(sortMe);
                sortMe.currentCol++;
                break;
            case insertion:  
                insertionSort(sortMe);
                sortMe.currentCol++;
                break;
            case cocktail:  
                cocktailSort(sortMe);
                break;
            case merge:  
                mergeSort(sortMe, 0, SIZE - 1);
                break;
            case heap:  
                heapSort(sortMe);
                break;
            default:   
                return ERROR;
        }

        //  end the current frame
        window.display();

        //  if sorted starts 10s timer and then closes the window
        if (sortMe.sorted) {
            stopwatch(sortMe);
            sleep(5);
            break;
        }
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


/*
     #######    ### ##    ##   ###    #######  ###   
    ##     ##  ##   ###   ##  ## ##  ##     ##   ##  
    ##     ## ##    ####  ## ##   ##        ##    ## 
    ##     ## ##    ## ## ##          #######     ## 
    ##     ## ##    ##  ####         ##           ## 
    ##     ##  ##   ##   ###         ##          ##  
     #######    ### ##    ##         ######### ###
*/


/*  
* Implementation of bubble sort
The bigger bubbles reach the top faster than smaller bubbles, and this algorithm works in the same way. 
It iterates through the data structure and for each cycle compares the current element with the next one, 
swapping them if they are in the wrong order.
*/
void bubbleSort(SortMe &sortMe)
{
    for (int i = 0; i < (SIZE - 1); i++) {
        if (sortMe.arr[i] > sortMe.arr[i + 1]) {
            for (; i < (SIZE - sortMe.currentCol - 1); i++) {
                if (sortMe.arr[i] > sortMe.arr[i + 1]) {
                    std::swap(sortMe.arr[i], sortMe.arr[i + 1]);
                }
            }
            return;
        }
    }
}


/*
* Implementation of selection sort:
Selection Sort is an iterative and in-place sorting algorithm 
that divides the data structure in two sublists: the ordered one, and the unordered one. 
The algorithm loops for all the elements of the data structure and for every cycle picks 
the smallest element of the unordered sublist and adds it to the sorted sublist, progressively filling it.
*/
void selectionSort(SortMe &sortMe)
{
    int minIndex;
    for (int i = sortMe.currentCol; i < SIZE - 1; i++) {
        minIndex = i;
        for (int j = i + 1; j < SIZE; j++)
          if (sortMe[j] < sortMe[minIndex]) minIndex = j;
        std::swap(sortMe[minIndex], sortMe[i]);
        return;
    }
}


/*
* Implementation of insertion sort
The algorithm divides the data structure in two sublists: a sorted one, and one still to sort. 
Initially, the sorted sublist is made up of just one element and it gets progressively filled. 
For every iteration, the algorithm picks an element on the unsorted sublist and inserts it at 
the right place in the sorted sublist.
*/
void insertionSort(SortMe &sortMe)
{
    for (int i = sortMe.currentCol+1; i < SIZE; i++) {
        int j = i - 1, key = sortMe[i];
        for (; j >= 0 && sortMe[j] > key; j--) {
            sortMe[j + 1] = sortMe[j];
        }
        sortMe[j + 1] = key;
        return;
    }
}


/*
* Implementation of cocktail sort
Shaker Sort alternates two Bubble Sorts, the first one that sorts the structure starting 
from the largest element ordering the elements down to the smallest, and the second one, 
that starts from the smallest element and sorts the elements up to the largest.
*/
void cocktailSort(SortMe &sortMe)
{
    static int direction = 1, colsSorted = 0;
    int i = sortMe.currentCol;
    if (direction > 0) {
        if (sortMe.currentCol < SIZE - colsSorted - 1) {
            if (sortMe[i] > sortMe[i + 1]) std::swap(sortMe[i], sortMe[i + 1]);
            sortMe.currentCol++;
            return;
        }
        direction = -1;
    }
    else {
        if (sortMe.currentCol > 0 + colsSorted) {
            if (sortMe[i] > sortMe[i + 1]) std::swap(sortMe[i], sortMe[i + 1]);
            sortMe.currentCol--;
            return;
        }
        direction = 1;
        colsSorted++;
    }
}


/*
     #######    ### ##    ##    ##        #######   ######      ##    ## ###   
    ##     ##  ##   ###   ##    ##       ##     ## ##    ##     ###   ##   ##  
    ##     ## ##    ####  ##    ##       ##     ## ##           ####  ##    ## 
    ##     ## ##    ## ## ##    ##       ##     ## ##   ####    ## ## ##    ## 
    ##     ## ##    ##  ####    ##       ##     ## ##    ##     ##  ####    ## 
    ##     ##  ##   ##   ###    ##       ##     ## ##    ##     ##   ###   ##  
     #######    ### ##    ##    ########  #######   ######      ##    ## ###   
*/


// * Implementation of merge sort (1/2)
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
            leftIndex++;
        }
        else {
            sortMe[sortMe.currentCol] = rightArr[rightIndex];
            rightIndex++;
        }
        sortMe.currentCol++;
    }

    //  copy the remaining values
    while (leftIndex < leftSize) {
        sortMe[sortMe.currentCol] = leftArr[leftIndex];
        leftIndex++;
        sortMe.currentCol++;
    }

    while (rightIndex < rightSize) {
        sortMe[sortMe.currentCol] = rightArr[rightIndex];
        rightIndex++;
        sortMe.currentCol++;
    }

    delete[] leftArr;
    delete[] rightArr;
}


/*
* Implementation of merge sort (2/2)
The algorithm divides the data structure recursively until the subsequences contain only one element. 
At this point, the subsequences get merged and ordered sequentially. 
To do so, the algorithm progressively builds the sorted sublist by adding each time 
the minimum element of the next two unsorted subsequences until there is only one sublist remaining. 
This will be the sorted data structure.
*/
void mergeSort(SortMe &sortMe, const int &left, const int &right)
{
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(sortMe, left, mid);
    mergeSort(sortMe, mid + 1, right);
    mergeMe(sortMe, left, mid, right);
}


// * Implementation of heap sort (1/2)
void heapify(SortMe &sortMe, int size, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < size && sortMe[left] > sortMe[largest]) largest = left;
    if (right < size && sortMe[right] > sortMe[largest]) largest = right;
    if (largest != i) {
        std::swap(sortMe[i], sortMe[largest]);
        heapify(sortMe, size, largest);
    }
}


/*
* Implementation of heap sort (2/2)
The data structure gets ordered to form the heap initially, 
and then it gets progressively reordered with an algorithm similar to Selection Sort, 
starting from the bigger elements.
*/
void heapSort(SortMe &sortMe) 
{
    static int i;
    switch (sortMe.phase) {
        case 0:
            i = SIZE / 2 - 1;
            sortMe.phase++;
            return;
        case 1:  
            while (i >= 0) {
                heapify(sortMe, SIZE, i);
                i--;
                return;
            }
            sortMe.phase++;
            return;
        case 2:  
            i = SIZE - 1;
            sortMe.phase++;
            return;
        case 3:   
            while (i >= 0) {
                std::swap(sortMe[0], sortMe[i]);
                heapify(sortMe, i, 0);
                i --;
                return;
            }
    }
} 

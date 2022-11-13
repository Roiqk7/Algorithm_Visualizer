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
#define SIZE 250                        //  size of array to be sorted
#define COL_WIDTH 5                     //  width of col representing number in an array
#define COL_MULTIPLIER 2                //  multiplies col value
#define WIDTH SIZE*COL_WIDTH            //  width of the window
#define HEIGHT SIZE*COL_MULTIPLIER      //  height of the window

// return values
#define SUCCESS 0
#define ERROR -1

//  algorithm identification
#define sortAlgsCount 7
enum SortAlgsEnum {NONE, bubble, selection, insertion, cocktail, merge, heap, quick};
const std::string sortAlgsNames[sortAlgsCount] = {"bubble", "selection", "insertion", "cocktail", "merge", "heap", "quick"};

// create the window     
sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIDTH, HEIGHT)), "Sort Visualizer");

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
        std::array<sf::RectangleShape, SIZE> arrOfRects;

        SortMe() {
            arr = generateUnsortedArray();
            sorted = false;
        }

        //  returns arr[i] if user types sortMe[i]
        int &operator[](int i){return arr[i];}

        //  draws the array on screen
        void render()
        {
            window.clear();
            for (int i = 0; i < SIZE; i++) {
                arrOfRects[i].setPosition (sf::Vector2f(i*COL_WIDTH, HEIGHT-arr[i]*COL_MULTIPLIER));
                arrOfRects[i].setSize(sf::Vector2f(COL_WIDTH, arr[i]*COL_MULTIPLIER));
                if (i == currentCol && i != 0) arrOfRects[i].setFillColor(sf::Color::Green);
                else arrOfRects[i].setFillColor(sf::Color::White);
                window.draw(arrOfRects[i]);
            }
            window.display();
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
void quickSort(SortMe &sortMe, int low, int high);                  //  implements quick sort


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
        }
        while (selectedAlgorithm < 0 && selectedAlgorithm < sortAlgsCount);                                       

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
    window.setFramerateLimit (60);
    //  run the program as long as the window is open
    while (window.isOpen()) {
        //  check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            //  handles closure of the window
            if (event.type == sf::Event::Closed) window.close();
        }

        //  clear the window with black color
        window.clear(sf::Color::Black);

        //  sort
        switch (selectedAlgorithm) {     
            case bubble:                                    
                bubbleSort(sortMe);
                break;
            case selection: 
                selectionSort(sortMe);
                break;
            case insertion:  
                insertionSort(sortMe);
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
            case quick:
                quickSort(sortMe, 0, SIZE - 1);
                break;
            default:   
                return ERROR;
        }

        //
        sortMe.isSorted();
        //  if sorted starts 10s timer and then closes the window
        if (sortMe.sorted) {
            window.clear();
            for (int i = 0; i < SIZE; i++) {
                sortMe.arrOfRects[i].setFillColor(sf::Color::Green);
                window.draw(sortMe.arrOfRects[i]);
            }
            window.display();
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
            for (; i < (SIZE - 1); i++) {
                if (sortMe.arr[i] > sortMe.arr[i + 1]) {
                    std::swap(sortMe.arr[i], sortMe.arr[i + 1]);
                    sortMe.currentCol = i + 1;
                    sortMe.render();
                }
            }
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
        sortMe.currentCol = i;
        sortMe.render();
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
        sortMe.currentCol = i;
        sortMe.render();
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
    bool swapped = true;
    int start = 0, end = SIZE - 1;
    while (swapped) {
        swapped = 0;
        for (int i = start; i < SIZE; i++) {
            if (sortMe[i] > sortMe[i + 1]) {
                std::swap(sortMe[i], sortMe[i + 1]);
                sortMe.currentCol = i;
                sortMe.render();
                swapped = true;
            }
        }
        if (!swapped) break;
        swapped = false;
        end--;
        for (int i = end - 1; i >= start; --i) {
            if (sortMe[i] > sortMe[i + 1]) {
                std::swap(sortMe[i], sortMe[i + 1]);
                sortMe.currentCol = i;
                sortMe.render();
                swapped = true;
            }
        }
        start++;
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

    sortMe.render();
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
        sortMe.render();
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
    for (int i = SIZE / 2 - 1; i >= 0; i--) heapify(sortMe, SIZE, i);
    for (int i = SIZE - 1; i >= 0; i--) {
        std::swap(sortMe[0], sortMe[i]);
        sortMe.currentCol = i;
        heapify(sortMe, i, 0);
    }
}


// * Implementation of quick sort (1/2)
int partition (SortMe &sortMe, int low, int high)
{
    int pivot = sortMe[high];
    int i = (low - 1);

    for (int j = low; j <= high- 1; j++) {
        if (sortMe[j] <= pivot) {
            i++;
            std::swap(sortMe[i], sortMe[j]);
            sortMe.render();
        }
    }
    std::swap(sortMe[i + 1], sortMe[high]);
    sortMe.render();
    return (i + 1);
}


/*
* Implementation of quick sort (2/2)
This division in partitions is done based on an element, called pivot: 
all the elements bigger than the pivot get placed on the right side of the structure, 
the smaller ones to the left, creating two partitions. 
Next, this procedure gets applied recursively to the two partitions and so on.
*/
void quickSort(SortMe &sortMe, int low, int high)
{
    if (low >= high) return;

    int pivot = partition(sortMe, low, high);

    quickSort(sortMe, low, pivot - 1);
    sortMe.render();
    quickSort(sortMe, pivot + 1, high);
    sortMe.render();
}

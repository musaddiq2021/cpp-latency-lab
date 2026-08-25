#include <iostream>
#include <vector>
#include <list>
#include <chrono>

int main()

//   VECTOR //
{
    const int SIZE = 1'000'000;

    std::vector<int> numbers;
    std::list<int> numberList;
    
        for (int i = 0; i < SIZE; i++)
    {
        numbers.push_back(i);
        numberList.push_back(i);
    }
    auto start = std::chrono::high_resolution_clock::now();

    long long sum = 0;

    for (int number : numbers) 
    {
        sum += number;
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);


    // LISTS //

    long long listSum = 0;

    auto listStart =
        std::chrono::high_resolution_clock::now();

    for (int number : numberList)
    {
        listSum += number;
    }

    auto listEnd =
        std::chrono::high_resolution_clock::now();

    auto listDuration =
        std::chrono::duration_cast<std::chrono::microseconds>(
            listEnd - listStart
        );
    
        // RESULTS //
    std::cout << "Sum: " << sum << '\n';
    std::cout << "Vector Iteration"
            <<duration.count()
            <<"microseconds\n";

    std::cout << '\n';

    std::cout << "List sum: " << listSum << '\n';
    std::cout << "List iteration: "
              << listDuration.count()
              << " microseconds\n";
        
        
              return 0;
}




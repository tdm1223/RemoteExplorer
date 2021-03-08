#include"ThreadPool.h"
#include<iostream>

void Plus(int t)
{
    std::cout << "PLUS" << t << std::endl;
}

void Minus(int t)
{
    std::cout << "MINUS" << t << std::endl;
}

void Work(int t, int id)
{
    printf("%d second \n", id);
    std::this_thread::sleep_for(std::chrono::seconds(3 * t));
    printf("second %d end after %ds\n", id, 3 * t);
    printf("\n");
}

int main()
{
    ThreadPool pool(3);

    for (int i = 1; i <= 5; i++)
    {
        pool.EnqueueJob([i]() { Plus(i); });
        pool.EnqueueJob([i]() { Minus(i); });
    }
}
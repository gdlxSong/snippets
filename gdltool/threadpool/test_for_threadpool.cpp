
#include"threadpool.hpp"
#include"Timer.hpp"
#include<iostream>
#include<vector>
#include<atomic>


volatile std::atomic<int> Index = 0;

void func() {
       //maybe data race. cout.
       //std::cout << "Index = " << ++Index << std::flush << std::endl;
       Index++;
}

int main() {

       gdl::Timer timer;
       gdl::threadpool thpool(5);
       for (int i = 0; i < 1000; i++) {
              thpool.commit(func);
       }
       while (Index < 1000);
       std::cout << "elapsed " << timer.elapsed() << " milliseconds." << std::endl;
       return 0;
}
#include"iostream"
#include"timer.h"
#include<unistd.h>


void test0(){

	gdl::Timer timer;
	sleep(3);
	std::cout << "elapsed " << timer.elapsed() << " milliseconds." << std::endl;

}
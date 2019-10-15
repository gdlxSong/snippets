#include"objectpool.hpp"

#include<iostream>
#include"threadpool.hpp"
#include<windows.h>
#include"timer.hpp"


class String : public gdl::Clean {
public:
	String(int) : str(655350, 'a') {}
	void clean() { //str.clear();
	}
	std::string data() { return str; }
private:
	std::string str;
};

std::mutex mut;



void test_for_ObjectPool() {


	gdl::Timer timer;
	gdl::ObjectPool<String> objectPool(10, [] {return new String(10); });

	std::cout <<"对象池大小："<< objectPool.capcity() << std::endl;

	threadpool thpool(2);
	std::cout << "线程池池大小：" << thpool.idlCount() << std::endl;

	std::cout << "Init elapsed " << timer.elapsed() << " ms" << std::endl;
	timer.reset();
	for (int i = 0; i < 100000; i++) {
		thpool.commit([&] {

			//auto shptr = std::shared_ptr<String>(new String(20));
			std::shared_ptr<String> obj = objectPool.Get();
			//std::cout << obj->data() << std::endl;
			//do....
			
			});
	}

	while (thpool.idlCount() != 2) {
	}
	std::cout << "take task elapsed " << timer.elapsed() << " ms" << std::endl;




}
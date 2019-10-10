#include"aop.h"
#include<iostream>
#include<functional>


struct AA {


	void Before() {
		std::cout << "Before AA." << std::endl;
	}
	void After() {
		std::cout << "After AA." << std::endl;
	}
};

void test_for_aop() {

	int extra_parameter = 10;
	gdl::Invoke<AA>([] {std::cout << "核心逻辑在此." << std::endl; });
	gdl::Invoke<AA>([=] {[](int i) {std::cout << "核心逻辑在此. i = "<<i << std::endl; }(extra_parameter); });

}
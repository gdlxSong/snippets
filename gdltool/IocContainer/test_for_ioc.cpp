#include<iostream>
#include"IocContainer.hpp"

class A {
public:
	void test() { std::cout << "just a test from class A." << std::endl; }
};


struct Test {

	Test(int, A &a) {}
	Test(A*) {}
	void test() { std::cout << "just a test from struct Test." << std::endl; }
};


void test_for_ioc() {


	IocContainer ioc;
	ioc.RegisterType<Test, A>("test");
	auto test = ioc.ResolveShared<Test>("test");
	test->test();



	//类型检查比较严格的の.
	ioc.SimpleRegisterType<Test, int&&, A&&>("test2");
	A aa;
	try {
		std::cout << "------------test A&& ----------" << std::endl;
		auto test3 = ioc.ResolveShared<Test>("test2", 2, A());
		test3->test();
		auto test2 = ioc.ResolveShared<Test>("test2", 2, aa);
		test2->test();
		std::cout << "----------------end test --------" << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}



	ioc.SimpleRegisterType<A>("a");
	auto a = ioc.ResolveShared<A>("a");
	a->test();
}
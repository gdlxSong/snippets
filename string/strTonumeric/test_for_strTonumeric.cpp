
#include"Timer.hpp"
#include"strTonumeric.hpp"
#include<iostream>



int test0() {



	gdl::Timer timer;
	std::string transStr = "sadwa--++0x12dwad-dwa-0-0XE110";
	gdl::StrToNumeric::position_t pos = 0;
	auto x = gdl::StrToNumeric::strTonumeric<int, gdl::StrToNumeric::Base16>(transStr, nullptr);
	if (x)
		std::cout << "x = " << (*x) << std::endl;
	else
		std::cout << "catch exception." << std::endl;
	x = gdl::StrToNumeric::strTonumeric<int, gdl::StrToNumeric::Base16>(transStr, &pos);
	if (x)
		std::cout << "x = " << (*x) << std::endl;
	else
		std::cout << "catch exception." << std::endl;
	std::cout << "elapsed " << timer.elapsed_micro() << " miscroseconds." << std::endl;

	std::cout << std::is_floating_point<long double>::value << std::endl;


	return 0;
}
#include<iostream>
#include"range.hpp"






int test1() {
	//create and print range's elements.
	
       for (auto it : gdl::range(0, 9, 3))
              std::cout << it << " ";
       std::cout << std::endl;
       return 0;
}
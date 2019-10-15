

#ifndef TO_FUNCTION_HPP
#define TO_FUNCTION_HPP




#include<functional>
#include<tuple>

namespace gdl {

	template <class T>
	struct function_traits;

	//普通函数转std::function
	template<class Ret, class... Args>
	struct function_traits <Ret(Args...)> {

	public:
		enum { arity = sizeof...(Args) };
		typedef Ret function_type(Args...);
		typedef Ret return_type;
		using stl_function_type = std::function<function_type>;
		typedef Ret(*pointer)(Args...);

		template<size_t I>
		struct args
		{
			static_assert(I < arity, "Index is out of range, index must less than sizeof Args.");
			using type = typename std::tuple_element<I, std::tuple<Args...> >::type;
		};
	};



	//函数指针.
	template<class Ret, class... Args>
	struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

	//std::function
	template<class Ret, class... Args>
	struct function_traits<std::function<Ret(Args...)> > : function_traits<Ret(Args...)> {};


	//member function.
	#define FUNCTION_TRAITS(...)\
	template<class ReturnType, class ClassType, class... Args>\
	struct function_traits<ReturnType(ClassType::*)(Args...)__VA_ARGS__> : \
		function_traits<ReturnType(Args...)> {};\



	FUNCTION_TRAITS()
	FUNCTION_TRAITS(const)
	FUNCTION_TRAITS(volatile)
	FUNCTION_TRAITS(const volatile)


	//函数对象.
	template<class Callable>
	struct function_traits : function_traits<decltype(&Callable::operator())> {};


	//to_function
	template<class Function>
	typename function_traits<Function>::stl_function_type to_function(const Function& lambda) {
		return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
	}

	template<class Function>
	typename function_traits<Function>::stl_function_type to_function(Function&& lambda) {
		return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
	}

	template<class Function>
	typename function_traits<Function>::pointer to_function_pointer(const Function& lambda) {
		return static_cast<typename function_traits<Function>::pointer>(lambda);
	}



}



#endif
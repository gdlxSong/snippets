

#ifndef GDL_AOP_HPP
#define GDL_AOP_HPP


#include<type_traits>

namespace gdl {

/*
	struct noncopyable {

		noncopyable() = default;
		virtual ~noncopyable() = default;
		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable&) = delete;
	};
*/
#include"noncopyable.hpp"

#define HAS_MEMBER(member)\
template <class T, class... Args>struct has_member##member\
{\
private:\
	template<class U>static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type());\
	template<class U>static std::false_type Check(...);\
public:\
	enum{value = std::is_same<decltype(Check<T>(0)), std::true_type>::value};\
};\


HAS_MEMBER(Before)
HAS_MEMBER(After)


	template<class Func, class ... Args>
	struct Aspect :noncopyable
	{

		Aspect(Func&& f) : m_func(std::forward<Func>(f)) {}

		template<class T>
		typename std::enable_if<has_memberBefore<T, Args...>::value&& has_memberAfter<T, Args...>::value>::type Invoke(Args&& ...args, T&& aspect) {

			aspect.Before(std::forward<Args>(args)...);//核心逻辑之前的切面逻辑.
			m_func(std::forward<Args>(args)...);//核心逻辑.
			aspect.After(std::forward<Args>(args)...);//核心逻辑之后的切面逻辑.
		}

		template<class T>
		typename std::enable_if<has_memberBefore<T, Args...>::value && !has_memberAfter<T, Args...>::value>::type Invoke(Args&& ...args, T&& aspect) {

			aspect.Before(std::forward<Args>(args)...);//核心逻辑之前的切面逻辑.
			m_func(std::forward<Args>(args)...);//核心逻辑.
		}

		template<class T>
		typename std::enable_if<!has_memberBefore<T, Args...>::value&& has_memberAfter<T, Args...>::value>::type Invoke(Args&& ...args, T&& aspect) {

			m_func(std::forward<Args>(args)...);//核心逻辑.
			aspect.After(std::forward<Args>(args)...);//核心逻辑之后的切面逻辑.
		}

		template<class Head, class... Tail>
		void Invoke(Args&& ... args, Head&& headAspect, Tail&& ... tailAspect) {

			headAspect.Before(std::forward<Args>(args)...);
			Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
			headAspect.After(std::forward<Args>(args)...);
		}

	private:
		Func m_func;
	};

	template <class T> using identity_t = T;


	template<class... AP, class... Args, class Func>
	void Invoke(Func&& func, Args&& ... args) {

		Aspect<Func, Args...> asp(std::forward<Func>(func));
		asp.Invoke(std::forward<Args>(args)..., identity_t<AP>()...);
	}
}

#endif
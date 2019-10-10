/*

	@date: 2019.10.9
	@author: gdl
	@email: gdl_xxx@qq.com
	@history: 0.0.1
	@modify: 2019.10.9 - 17:13
	

	desc:
		实现Ioc的关键之处在于, Any和funcor的配合擦出类型，实现统一存储std::unordered_map<std::string, std::any> m_constructorMap;
		

	note:
		由于存储给中类型的构造器都是any，最后构造对象的时候都是需要std::any_cast来转换的,
		而在std::any_cast的转换时需要根据构造参数Args推导constructor(Args...)，
		在这是类型的检查都是严格的，否则无法获得理想的构造器.


		如在开始注册的时候：
			ioc.SimpleRegisterType<Test, int&&, float>("test1");
			那么：
				使用ioc构造对象的时候：
					只能是：ioc.ResolveShared<Test>("test1", 2, 2.3f);//这里2.3f不能是2.3
			如果：
			ioc.SimpleRegisterType<Test, int&&, A&&>("test2");
			那么：
				在使用ioc构造对象的时候：
					只能是auto test3 = ioc.ResolveShared<Test>("test2", 2, A());//A()是rvalue.

	example: 
		./test_for_ioc.cpp
*/







#ifndef GDL_IOCCONTAINER_HPP
#define GDL_IOCCONTAINER_HPP


#include"noncopyable.hpp"
#include<any>
#include<memory>
#include<string>
#include<unordered_map>
#include<functional>
#include<stdexcept>
#include<type_traits>
using namespace gdl;





class IocContainer : gdl::noncopyable {

public:
	IocContainer() {}
	~IocContainer() {}

	template<class DestType, class Depend, class... Args>
	typename std::enable_if<!std::is_base_of<DestType, Depend>::value>::type RegisterType(const std::string& key) {

		//通过闭包来戳掉参数类型. 去注册一个std::function<T*(Args...)>
		RegisterType(key, std::function<DestType* (Args...)>([](Args... args) {return new DestType(new Depend(args...)); }));
	}

	template<class DestType, class Depend, class... Args>
	typename std::enable_if<std::is_base_of<DestType, Depend>::value>::type RegisterType(const std::string& key) {

		//通过闭包来戳掉参数类型. 去注册一个std::function<T*(Args...)>
		RegisterType(key, std::function<DestType * (Args...)>([](Args... args) {return new Depend(args...); }));
	}

	template<class DestType, class... Args>
	void SimpleRegisterType(const std::string& key) {

		RegisterType(key, std::function<DestType * (Args...)>([](Args... args) {return new DestType(args...); }));
	}


	template<class T, class... Args>
	T* Resolve(const std::string& key, Args&&... args) {

		auto iter = m_constructorMap.find(key);
		if (m_constructorMap.end() == iter)
			return nullptr;
		try {
			auto creator = std::any_cast<std::function<T * (Args&&...)>>(iter->second);	//注意这里是按照Args&&来推导的.
			return creator(std::forward<Args>(args)...);
		}
		catch (std::exception& e) {
			throw std::invalid_argument("Constructor arguments type error.");
		}
		return nullptr;
	}

	template<class T, class... Args>
	std::shared_ptr<T> ResolveShared(const std::string& key, Args&&... args) {

		return std::shared_ptr<T>(Resolve<T>(key, std::forward<Args>(args)...));
	}


private:

	void RegisterType(std::string key, std::any&& constructor) {

		if (m_constructorMap.end() != m_constructorMap.find(key))
			throw std::invalid_argument("this key has aready exist.");
		m_constructorMap.emplace(key, std::move(constructor));
	}
	std::unordered_map<std::string, std::any> m_constructorMap;
};





#endif
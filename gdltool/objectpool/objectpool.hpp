
/*

	#title: Object pool.
	@desc: 对象池，以池化的思想来减少对象的申请和释放操作.
	@author: gdl.
	@version: 0.0.1
	@date: 2019.10.15 22:43
	@modify: $date
	@reference: 《深入应用c++11》 - 祁宇 - p238.

*/












#ifndef OBJECTPOOL_GDL_HPP
#define OBJECTPOOL_GDL_HPP

//对象池设计：
/*
	@需求：
		1. 对象池可以接受任意构造函数.
		2. 对象池大小可以动态调整.

	@使用要求：
		1. 对象池对外提供接口：
			ObjectPool(size_t n = BASE_OBJECT_SIZE, Constructor&& constructor = [] {return new ObjectClass; }): 
				parameter:
					n:size_t ：对象池的初始大小, 
					constructor:Constructor&& ：对象池对象存储的对象构造器，(默认为默认构造.)
		
			inline std::shared_ptr<T> ObjectPool<T>::Get()
				parameter:
					返回一个对象.

		2. 对象池存储的class必须具有clean成员函数，建议继承Class Clean.

		3. 使用#define AUTO_EXTEND开启对象池的自动扩涨功能，当对象不足时自动新增对象. 默认关闭此功能呢.(ps: 自动调度这块不够完善.)

	@note:
		对象池在大多数时候只适用于大对象。

	&also:
		还可以契合IOC模型，来扩展任意类型对象对象池<typeId, Object>; 但是这样做接口会变得负责，所以先不这样做.
*/




#include<list>
#include<memory>
#include<atomic>
#include<mutex>
#include<functional>
#include<stdexcept>
#include<string>
#include<map>
#include<condition_variable>
#include<type_traits>
#include"function_traits.hpp"




namespace gdl {

#define MAX_OBJECT_SIZE	2000
#define BASE_OBJECT_SIZE 10



#define HAS_MEMBER(member)\
template <class T, class... Args>struct has_member##member\
{\
private:\
	template<class U>static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type());\
	template<class U>static std::false_type Check(...);\
public:\
	enum{value = std::is_same<decltype(Check<T>(0)), std::true_type>::value};\
};\


	HAS_MEMBER(clean)

/*
	class Counter {
		using Trigger = std::function<void()>;
	public:
		Counter(int n) : count(0), number(n) {};
		void setNumber(int n) { 
			if (n > MAX_OBJECT_SIZE)
				throw std::invalid_argument("invalid arguments. ");
			number = n; 
		}
		void trigger(int c, Trigger&& cb) {
			
		}
	private:
		int count;
		int number;
	};

*/

	class Clean {

	public:
		Clean() = default;
		virtual~Clean() = default;
		virtual void clean() = 0;
	};







	template<class T>
	class ObjectPool {

		using Constructor = std::function<T* (void)>;
	public:
		template<class ObjectClass = T>
		ObjectPool(size_t n = BASE_OBJECT_SIZE, Constructor&& constructor = [] {return new ObjectClass; }) : _cap(n), creator(constructor)
		{
			if (n > MAX_OBJECT_SIZE)
				throw std::invalid_argument("invalid arguments, n must less than " + std::to_string(MAX_OBJECT_SIZE));
			this->alloc(n);
			exited = false;
		}



		inline std::shared_ptr<T> Get()
		{
			std::unique_lock<std::mutex> lock(mut);
#ifdef AUTO_EXTEND
			extend();
#endif
			cv.wait(lock, [this] { return size() > 0; });
			std::shared_ptr<T> shptr = pool.front();
			pool.pop_front();
			return shptr;
		}



		template<class M = T>
		inline typename std::enable_if<!has_memberclean<M>::value>::type alloc(size_t n)
		{
			throw std::runtime_error("ObjectPool<T>, class T must has function member: T::clean.");
		}

		template<class M = T>
		inline typename std::enable_if<has_memberclean<M>::value>::type alloc(size_t n)
		{
			for (size_t i = 0; i < n; i++) {
				pool.emplace_back(std::shared_ptr<T>(creator(), [this](T* ptr) {
					deletor(ptr);
					}));
			}
		}
		~ObjectPool() { exited = true; };
		size_t size() { return pool.size(); }
		size_t capcity() { return _cap; }
	private:

		inline void extend()
		{
			if (0 == size()) {
				if (MAX_OBJECT_SIZE > capcity()) {
					size_t n = MAX_OBJECT_SIZE - capcity();
					n = n > BASE_OBJECT_SIZE ? BASE_OBJECT_SIZE : n;
					alloc(n);
					_cap += n;
					cv.notify_all();
				}
			}
		}

		void deletor(T* ptr) {
			if (exited) {
				delete ptr;
				return;
			}
			//T对象必须具有回收自清理函数, 接口. 从而使得其在回收后可以复用.
			ptr->clean();
			std::unique_lock<std::mutex> lock(mut);
			pool.emplace_back(std::shared_ptr<T>(ptr, [this] (T* p){
				deletor(p);
				}));
			lock.unlock();
			cv.notify_one();
		}


	private:
		bool exited;
		size_t _cap;
		std::mutex mut;
		std::condition_variable cv;
		Constructor creator;
		std::list<std::shared_ptr<T> > pool;
	};












}





#endif
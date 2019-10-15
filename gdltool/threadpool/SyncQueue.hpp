



#ifndef SYNC_QUEUE_HPP
#define SYNC_QUEUE_HPP



#include<list>
#include<mutex>
#include<condition_variable>
#include"util.hpp"


namespace gdl {



	template<class T>
	class SyncQueue : gdl::noncopyable {

	public:
		SyncQueue() : stoped(false) {}

		void push(const T& task) {
			std::lock_guard<std::mutex> lock(mut);
			if (stoped)
				return;
			tasks.emplace_back(task);
			cv.notify_one();
		}
		void push(T&& task) {
			std::lock_guard<std::mutex> lock(mut);
			if (stoped)
				return;
			tasks.emplace_back(std::forward<T&&>(task));
			cv.notify_one();
		}

		bool pop(T& task) {
			std::unique_lock<std::mutex> lock(mut);
			cv.wait(lock, [this]() { return !this->empty() || this->stoped.load(); });
			if (!stoped) {//not stoped.
				task = std::move(tasks.front());
				tasks.pop_front();
				return true;
			}
			return false;
		}

		bool empty() {
			return tasks.empty();
		}

		void stop() {
			{
				std::lock_guard<std::mutex> lock(mut);
				stoped = true;
			}
			cv.notify_all();
		}

		size_t count() {
			return tasks.size();
		}

	private:
		std::atomic<bool> stoped;
		std::mutex mut;
		std::condition_variable cv;
		std::list<T> tasks;
	};





}






#endif
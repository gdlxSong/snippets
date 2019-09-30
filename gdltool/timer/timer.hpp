#ifndef GDL_TIMER_H
#define GDL_TIMER_H

#include<chrono>


namespace gdl {

	//需要注意的是：GCC低版本，如v4.x.x不支持std::put_time， 测试了一下，我的gcc7.3.1是支持的.

	class Timer {

	public:
		//construct Timer and initializer.
		Timer() : m_begin(std::chrono::high_resolution_clock::now()){}
		void reset() {
			m_begin = std::chrono::high_resolution_clock::now();
		}

		//milliseconds.
		template<class Duration = std::chrono::milliseconds>
		int64_t elapsed()const {
			return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}

		//microseconds.
		int64_t elapsed_miscc()const {
			return elapsed<std::chrono::microseconds>();
		}

		//nanoseconds
		int64_t elapsed_nano()const {
			return elapsed<std::chrono::nanoseconds>();
		}

		//seconds
		int64_t elapsed_seconds()const {
			return elapsed<std::chrono::seconds>();
		}

		//minutes
		int64_t elapsed_minutes()const {
			return elapsed<std::chrono::minutes>();
		}

		//hours
		int64_t elapsed_hour()const {
			return elapsed<std::chrono::hours>();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	};




}



#endif

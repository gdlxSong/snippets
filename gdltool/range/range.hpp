#include<exception>
#include<algorithm>
namespace gdl {
       
       ////////////////////
       //The range Iterator.
       ////////////////////
       template<class T>
       class iterator {
       public:
              using value_type = T;
              using size_type = size_t;
       private:
              size_type cursor_;
              const value_type step_;
              value_type value_;
       public:
              iterator(size_type cur_start, value_type begin_val, value_type step_val) :
                     cursor_(cur_start),
                     step_(step_val),
                     value_(begin_val)
              {
                     value_ += (step_ * cursor_);
              }
              value_type operator*()const {
                     return value_;
              }
              bool operator!=(const iterator& rhs)const {
                     return cursor_ != rhs.cursor_;
              }
              //前置++
              iterator& operator++(void) {
                     value_ += step_;
                     ++cursor_;
                     return (*this);
              }
       };
       //////////////////
       //The impl class
       //////////////////
       template<class T>
       class impl {
       public:
              using value_type = T;
              using reference = const value_type &;
              using const_reference = const value_type &;
              using iterator = const gdl::iterator<value_type>;
              using const_iterator = const gdl::iterator<value_type>;
              using size_type = typename iterator::size_type;
       private:
              const value_type begin_;
              const value_type end_;
              const value_type step_;
              const size_type max_count_;
              size_type get_adjusted_count(void) const {
                     if (step_ > 0 && begin_ >= end_)
                           throw std::exception("End value must be greater than begin value.");
                     else if (step_ < 0 && begin_ <= end_)
                           throw std::exception("End value must be less than begin value.");
                     size_type x = static_cast<size_type>((end_ - begin_) / step_);
                     if (begin_ + (step_ * x) != end_)
                           ++x;
                     return x;
              }
       public:
              impl(value_type begin_val, value_type end_val, value_type step_val)
                     : begin_(begin_val)
                     , end_(end_val)
                     , step_(step_val)
                     , max_count_(get_adjusted_count())
              {
                     //do nothing...
              }
              size_type size()const {
                     return max_count_;
              }
              const_iterator begin()const {
                     return {0, begin_, step_};
              }
              const_iterator end()const {
                     return {max_count_, begin_, step_};
              }
       };
       template<class T>
       impl<T> range(T end) {
              return { {}, end, 1 };
       }
       template<class T>
       impl<T> range(T begin, T end) {
              return { begin, end, 1 };
       }
       template<class T, class U>
       auto range(T begin, T end, U step) -> impl<decltype(begin + step)> {
              //may be narrowing....
              using r_t = gdl::impl<decltype(begin + step)>;
              return r_t(begin, end, step);
       }
}

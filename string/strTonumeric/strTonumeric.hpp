
/*

	此函数库提供从string中提取numeric.
	并频闭异常的抛出.
	这个库依赖于<string>标准函数库.
	实现：stoi, stol, stoul, stoll, stoull, stof, stod, stold

	//对sto*:
		sto*函数而言，stoi,stol等转化为整数，接受的字符串只能由数字或者0x等开始，stof,stod等转化为浮点数只能由'.'或数字开始。
		sto*族函数第二个参数idx是一个输出参数。

	所以strTonumeric肩负在字符串中找到数字的重任 => size_t std::string::find_first_of (const string& str, size_t pos = 0) const noexcept;


	bug: 函数只是实现了string，但是没有兼容wstring

	note: std::string::find_first_of(const string& str, size_t pos = 0)返回值与pos参数无关.


	-----------------
	Interface:
		[]strtolower:
			declare:
				std::string strtolower(const std::string& str)
			parameters:
				str : const std::string
					需要进行大写到小写转换的字符串.
			return:	
					返回转换为小写的字符串：std::string


		[]strTonumeric<destT, Index>
			declare:
				template<class destT, NumericType Index, class String = std::string>
				typename std::enable_if<std::is_same<ClassType, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0)
			desc:
				函数模板接受两个模板参数：
					destT:是用户想要从字符串中提取的类型 destT -> {int, long, unsigned long, long long, unsigned long long, float, double, long double}
					Index:是提取的内容在字符串中的存在形式：如十进制，十六进制...  本库暂时支持 Base2, Base8, Base10, Base16, Float, 当然使用者可以自行扩展.

			parameters:
				str : const String&	:	要提取的字符串.
				ppos : size_t *		:	字符串的操作范围 [&str[0]+*ppos, ~]

			note:	值得注意的是，本函数暂时只支持std::string的str输入
*/




#ifndef GDL_STRTRANSFER_H
#define GDL_STRTRANSFER_H
#include<exception>
#include<string>
#include<optional>
#include<ctype.h>
#include<functional>
#include<type_traits>
#include<memory>

namespace gdl {


#ifndef __in__
#define __in__
#endif
#ifndef __out__
#define __out__
#endif
#ifndef __inout__
#define __inout__
#endif


	std::string strtolower(const std::string& str) noexcept {
		std::string s = str;
		for (auto& it : s) {
			if ('A' <= it && it <= 'Z')
				it += 32;
		}
		return s;
	}



	namespace StrToNumeric {

#define Flag_Float (-1)


		using position_t = size_t;
		enum Signal { Negative, Positive };
		enum NumericType { Base2, Base8, Base10, Base16, Float, Other};


		Signal checkSignal(const std::string& str, position_t pos) {

			if (0 == pos)
				return Positive;
			else {
				if ('-' == str[pos - 1])
					return Negative;
				return Positive;
			}
		}

		template<NumericType Index>
		bool checkChar(const std::string& str, position_t pos, std::string& pattern) {

			//检查其后格式位0后面是否有01
			if (pos + 1 >= str.size())
				return false;
			else {

				if (std::string::npos == pattern.find(str[pos + 2]))
					return false;
				return true;
			}
		}

		//十进制
		template<NumericType Index>
		typename std::enable_if<Index == Base10, bool>::type checkChar(const std::string& str, position_t pos) {
			return true;
		}

		//二进制
		template<NumericType Index>
		typename std::enable_if<Index == Base2, bool>::type checkChar(const std::string& str, position_t pos) {
			return true;
		}

		//十六进制
		template<NumericType Index>
		typename std::enable_if<Index == Base16, bool>::type checkChar(const std::string& str, position_t pos) {

			std::string pattern = "0123456789abcdef";
			return checkChar<Index>(str, pos, pattern);
		}

		//Floating point
		template<NumericType Index>
		typename std::enable_if<Index == Float, bool>::type checkChar(const std::string& str, position_t pos) {

			std::string pattern = "0123456789";
			if ('.' != str[pos])
				return true;
			return checkChar<Index>(str, pos, pattern);
		}


		template<NumericType Index>
		std::optional<position_t> findNumeric(const std::string& str, const std::string& match, size_t* ppos) {

			//对于有前缀的格式数我们寻找其前缀，没有格式直接寻找其内容，有格式的需要check.
			auto index = str.find_first_of(match, *ppos);

			if (std::string::npos == index) {
				//not find. and set ppos.
				*ppos = str.size();
				return std::nullopt;
			}
			else {

				if (checkChar<Index>(str, index)) {
					return std::optional<position_t>(index);
				}
				else {
					//checkChar faild.//set ppos.
					*ppos = index + 1;
					return std::nullopt;
				}

			}

		}

		


		//10进制
		template<NumericType Index>
		typename std::enable_if<Index == Base10, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			//从字符串中寻找十进制，八进制，二进制...等数字的起始索引位置
			//ppos贯穿两个函数，但是，只是在这里做修改.
			std::string match = "0123456789";
			return findNumeric<Index>(str, match, ppos);
		}

		//2进制
		template<NumericType Index>
		typename std::enable_if<Index == Base2, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			//从字符串中寻找十进制，八进制，二进制...等数字的起始索引位置
			//ppos贯穿两个函数，但是，只是在这里做修改.
			std::string match = "01";
			return findNumeric<Index>(str, match, ppos);
		}

		////16进制
		//template<NumericType Index>
		//typename std::enable_if<Index == Integer16, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

		//	//十六进制需要将str转小写
		//	
		//	std::string match = "0x";
		//	return findNumeric<Index>(gdl::strtolower(str), match, ppos);
		//}
		//16进制
		template<NumericType Index>
		typename std::enable_if<Index == Base16, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			std::string match = "0x";
			//对于有前缀的格式数我们寻找其前缀，没有格式直接寻找其内容，有格式的需要check.
			//需要将字符串统一成小写
			std::string s = gdl::strtolower(str);
			auto index = s.find(match, *ppos);

			if (std::string::npos == index) {
				//not find. and set ppos.
				*ppos = s.size();
				return std::nullopt;
			}
			else {

				if (checkChar<Base16>(s, index)) {
					return std::optional<position_t>(index);
				}
				else {
					//checkChar faild.//set ppos.
					*ppos = index + 1;
					return std::nullopt;
				}

			}

		}

		//Floating point
		template<NumericType Index>
		typename std::enable_if<Index == Float, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			

			std::string match = ".0123456789";
			return findNumeric<Index>(str, match, ppos);
		}





		template <class T, class destT, class String = std::string, class Func>
		typename std::enable_if<std::is_floating_point<T>::value, T>::type stoxxx(Func&& f, const String& __in__ str, size_t* __inout__ ppos = 0, int base = 10) {

			return f(str.c_str(), ppos);
		}
		template <class T, class String = std::string, class Func>
		typename std::enable_if< !std::is_floating_point<T>::value, T>::type stoxxx(Func&& f, const String& __in__ str, size_t* __inout__ ppos = 0, int base = 10) {

			return f(str.c_str(), ppos, base);
		}


		constexpr int getBase(NumericType Index) {

			switch (Index)
			{
			case Base2:
				return 2;
			case Base8:
				return 8;
			case Base10:
				return 10;
			case Base16:
				return 16;
			case Float:
				return -1;
			default:
				return 0;
			}
		}

		//std::stoxxx
		template<NumericType Index, class destT, class String = std::string, class Func>
		std::optional<destT> strTonumeric(Func&& f, const String& __in__ str, size_t* __inout__ ppos = 0) noexcept {


			using valueType = destT;

			//因为ppos会在后面的调用中使用. 所以不能为null, 所以动态申请一个，交给shared_ptr就行了
			std::shared_ptr<size_t> shaptr(new size_t{0});
			if (nullptr == ppos) {
				ppos = shaptr.get();
			}

			


			//str2int.
			try {
				//find numeric from &str[0]+*ppos
				std::optional<position_t> pos = findNumeric<Index>(str, ppos);//Index是编译器参数.
				//check pos.
				while (*ppos < str.size()) {
					if (pos)
						break;
					pos = findNumeric<Index>(str, ppos);
				}
				//check pos or (*ppos >= str.size())
				if (!pos)
					return std::nullopt;

				//ppos 是一个__in__, __out__ 参数
				//判断符号.
				int s = Positive == checkSignal(str, pos.value()) ? 0 : 1;
				valueType value = stoxxx<destT, String, Func>(std::forward<Func>(f), str.c_str() + pos.value() - s, ppos, getBase(Index));
				//set ppos.
				*ppos = *ppos + pos.value() - s;
				return std::optional<valueType>(value);
			}
			catch (...) {

				return std::nullopt;
			}

		}

		//std::stoi
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<int, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, int(*)(const std::string&, size_t*, int)>(&std::stoi, str, ppos);
		}

		//std::stol
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<long, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, long(*)(const std::string&, size_t*, int)>(&std::stol, str, ppos);
		}



		//std::stoul
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<unsigned int, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, unsigned int(*)(const std::string&, size_t*, int)>(&std::stoul, str, ppos);
		}


		//std::stoll
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<long long, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, long long(*)(const std::string&, size_t*, int)>(&std::stoll, str, ppos);
		}


		//std::stoull
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<unsigned long long, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, unsigned long long(*)(const std::string&, size_t*, int)>(&std::stoull, str, ppos);
		}



		//std::stof
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<float, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, float(*)(const std::string&, size_t*)>(&std::stof, str, ppos);
		}


		//std::stod
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<double, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, double(*)(const std::string&, size_t*)>(&std::stod, str, ppos);
		}


		//std::stold
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<long double, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, long double(*)(const std::string&, size_t*)>(&std::stold, str, ppos);
		}

	}





}

#endif
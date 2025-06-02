#pragma once

#include <array>
#include <iostream>
#include <sstream>
namespace zm {
	class printer {
		public:
			printer(const char *str);
			~printer();
			template<typename... Args>
			void print(const char *str, const Args &...args) {
				collect(args...);
				_print(str);
			}
		private:
			void _print(const char *str);
			std::stringstream _converter;
			template<typename head, typename... Args>
			void collect(const head &h, const Args &...args) {
				_converter << h;
				_cash.at(_cash_index) = _converter.str();
				_converter.str("");
				_cash_index++;
				collect(args...);
			}
			void collect(){}
			std::string parse(const char *str);
			

			std::stringstream _output;
			std::array<std::string, 9> _cash;
			std::string _config;
			int _cash_index = 0;

			static const std::chrono::time_point<std::chrono::steady_clock> _start_time;
	};

#define ZMPRINTER_IMPL
#ifdef ZMPRINTER_IMPL
#include <string_view>
	const std::chrono::time_point<std::chrono::steady_clock> printer::_start_time = std::chrono::steady_clock::now();
	printer::printer(const char *str)
		:_config(str){

	}
	printer::~printer() {
	}
	std::string printer::parse(const char *str) {
		static std::stringstream message;
		message.str("");
		unsigned index=0;
		for(unsigned i=0;; i++) {
			if(str[i] == '\0') {
				message << std::string_view(&str[index], i-index);
				break;
			}
			if(str[i] == '{' && str[i+1] == '%' && std::isdigit(str[i+2]) && str[i+3] == '}') {
				message << std::string_view(&str[index], i-index);
				message << _cash[str[i+2]-'0'];
				index = i+4;
				i = index;
			}
		}
		return message.str();
	}
	void printer::_print(const char *str) {
		unsigned index = 0;
		for(unsigned i=0;;i++) {
			if(_config[i] == '\0') {
				_output << std::string_view(&_config[index], i-index);
				break;
			}
			if(_config[i] == '{' && _config[i+1] == '%' && _config[i+2] == 't' && _config[i+3] == '}') {
				_output << std::string_view(&_config[index], i-index);
				index = i+4;
				i = index;
				auto end_time = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

				int total_seconds = elapsed.count();
				int hours = total_seconds / 3600;
				int minutes = (total_seconds % 3600) / 60;
				int seconds = total_seconds % 60;
				_output << hours << ':' << minutes << ':' << seconds;
			}
			else if(_config[i] == '{' && _config[i+1] == '%' && _config[i+2] == 'm' && _config[i+3] == '}') {
				_output << std::string_view(&_config[index], i-index);
				index = i+4;
				i = index;
				_output << parse(str);
			}
		}
		std::cout << _output.str();
		_output.str("");
		_cash_index = 0;
	}
#endif //ZMPRINTER_IMPL
}

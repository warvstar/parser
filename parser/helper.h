#pragma once
#include "std.core.h"
#include "tokens.h"
using namespace std::chrono;
enum SettingWhat {
	NONE, NAME, TYPE, VALUE
};

struct Helper {
	static long calculateCallOperand(uint8_t* funcAddress, uint8_t* currentAddress);
	template <typename T>
	static inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
		return ss.str();
	}
	static std::string timer;
	static std::string readFile(std::string_view filename);
	static std::vector<std::pair<uint32_t, uint32_t>> split_string(std::string_view str, std::string_view delimiter);
	static size_t find_any(std::string_view str, std::vector<std::string_view>& delimiters, size_t prev, size_t end);
	static std::vector<std::pair<uint32_t, uint32_t>> split_line(std::string_view str, uint32_t start, uint32_t end, std::vector<std::string_view> delimiters);
	static void replaceAll(std::string& str, const std::string& from, const std::string& to);
	//static void parse(UnparsedToken& token, SettingWhat& what, std::string_view str);
	static std::string toLua(std::string_view str);
	//static void parseArgs(UnparsedToken& token, std::string_view str, uint32_t& i, uint32_t end);
	static bool splitTokens(std::vector<std::unique_ptr<Token>>& tokens, std::string_view str, uint32_t& start, uint32_t end);
	//slower by 30% than the split_string method but produces string_view
	static std::vector<std::string> split(std::string_view strv, std::string_view delims = " ");
	//slower by 2x than the split_string method but produces strings
	static std::vector<std::string_view> splitSV(std::string_view strv, std::string_view delims = " ");
	static std::string GetSpaces(uint16_t level);
};
#define TIMER_START(t) { Helper::timer = t; high_resolution_clock::time_point t1 = high_resolution_clock::now();
#define TIMER_END high_resolution_clock::time_point t2 = high_resolution_clock::now(); duration<double, std::milli> time_span = t2 - t1; std::cout << std::endl; std::cout << "It took me " << time_span.count() << "/ms to do:" << Helper::timer << std::endl;}

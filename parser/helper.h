#pragma once
#include "std.core.h"
#include <cctype>
#include "tokens.h"
using namespace std::chrono;
enum SettingWhat {
	NONE, NAME, TYPE, VALUE
};

struct Helper {
	static bool IsNaN(const std::string& str) {
		if (str.size() > 0)
			if (!isdigit(str[0]))
				return true;
		auto it = std::find_if(std::begin(str), std::end(str), [](char c) {
			return !std::isdigit(c);
		});
		return it != std::end(str);
	}
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
	static std::vector<std::pair<uint32_t, uint32_t>> split_line(std::string_view str, uint32_t start, uint32_t end, std::vector<std::string_view> delimiters);
	static void replaceAll(std::string& str, const std::string& from, const std::string& to);
	//static void parse(UnparsedToken& token, SettingWhat& what, std::string_view str);
	static std::string toLua(std::string_view str);
	static bool splitTokens(std::vector<std::unique_ptr<Token>>& tokens, std::string_view str);
	//static void parseArgs(UnparsedToken& token, std::string_view str, uint32_t& i, uint32_t end);
	//slower by 30% than the split_string method but produces string_view
	static std::vector<std::string> split(std::string_view strv, std::string_view delims = " ");
	//slower by 2x than the split_string method but produces strings
	static std::vector<std::string_view> splitSV(std::string_view strv, std::string_view delims = " ");
	static std::string GetSpaces(uint16_t level);
	static std::vector<std::string> split2(std::string_view s, const std::string& c) {
		std::string buff{ "" };
		std::vector<std::string> v;

		for (int i = 0; i < s.size(); ++i) {
			auto n = s[i];
			if (n != c[0]) buff += n; else
				if (n == c[0] && s[i + 1] == c[1] && s[i + 2] == c[2] && buff.size() != 0) {
					v.push_back(buff);
					buff = "";
				}
		}
		if (buff != "") v.push_back(buff);

		return v;
	};
	static inline std::vector<std::string> splitString(std::string_view s, const std::string& delimiter) {
		std::vector<std::string> ret;
		size_t start = 0;
		size_t end = 0;
		size_t len = 0;
		std::string token;
		do {
			end = s.find(delimiter, start);
			len = end - start;
			token = s.substr(start, len);
			ret.emplace_back(token);
			start += len + delimiter.length();
		} while (end != std::string::npos);
		return ret;
	}
	static std::vector<std::string> split_1(std::string_view s, const char c) {
		std::string buff{ "" };
		std::vector<std::string> v;

		for (int i = 0; i < s.size(); ++i) {
			auto n = s[i];
			if (n != c) buff += n; else
				if (n == c && buff.size() != 0) {
					v.push_back(buff);
					buff = "";
				}
		}
		if (buff != "") v.push_back(buff);

		return v;
	};
	static bool getBetween(std::string& str, std::string& replaced, const std::string& s, const std::string& e) {
		if (str.empty())
			return false;
		int foundStart = str.find(s);
		if (foundStart > -1) {
			int foundEnd = str.substr(foundStart).find(e);
			if (foundEnd > -1) {
				int start = foundStart + s.size();
				int end = foundEnd - s.size();
				replaced = str.substr(start, end);
				str.replace(foundStart, foundEnd, "");
				return true;
			}
		}
		return false;
	}
	static bool BasicTypeCheck(const std::string& type);
	static bool BasicTypeCheck(const Type& type);
	static DemangledStruct Demangle(const std::string& str, std::vector<std::string> vec, const std::string& body);
	static std::pair<std::string, std::string> ModifyMangled(const std::string& in, const DemangledStruct& demangled);
	static std::string Mangle(DemangledStruct& demangled);
};
#define TIMER_START(t) { Helper::timer = t; high_resolution_clock::time_point t1 = high_resolution_clock::now();
#define TIMER_END high_resolution_clock::time_point t2 = high_resolution_clock::now(); duration<double, std::milli> time_span = t2 - t1; std::cout << std::endl; std::cout << "It took me " << time_span.count() << "/ms to do:" << Helper::timer << std::endl;}

#include <string_view>
#include <iostream>
#include <unordered_map>
#include <vector>
struct Option {
	std::string error = "";
	std::string value = "";
};
std::unordered_map<std::string, Option> options = { {"-o", { "%s is missing value.\n", "a.out"} } };
std::vector<std::string> files;
int main(int argc, char* argv[]) {
	auto check_for_val = [&](int& i, const std::string& v_name, Option& v, char* argv[]) {
		if (i+2 > argc) {
			printf(v.error.data(), v_name.data());
			return (char*)nullptr;
		}

		auto o = argv[++i];
		if (o[0] == '-') {
			i--;
			printf(v.error.data(), v_name.data());
			return (char*)nullptr;
		}
		return o;
	};
	if (argc == 1) {
		std::cout << "The FutureScript compiler." << std::endl;
	}
	for (int i = 1; i < argc; ++i) {
		auto v = argv[i];
		if (v[0] == '-') {
			auto got = options.find(v);
			if (got != options.end()) {
				auto a  = check_for_val(i, got->first, got->second, argv);
				if (a)
					got->second.value = a;
			}
			else {
				std::cout << "Unrecognized option: " << v << std::endl;
			}
		}
		else {
			files.push_back(v);
			std::cout << "Adding file: " << v << std::endl;
		}
	}
	std::cout << "Output file: " << options["-o"].value << std::endl;
	return 1;
}
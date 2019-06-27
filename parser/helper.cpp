#include "helper.h"
#include "parser.h"
#include "tokens.h"
#include "vm.h"
long Helper::calculateCallOperand(uint8_t* funcAddress, uint8_t* currentAddress) {
	return (funcAddress - currentAddress) - 5;
}
std::string Helper::readFile(std::string_view filename) {
	std::string path = filename.data();
	std::string buffer = "";
	if (std::ifstream t{ path.c_str(), std::ifstream::binary }) {
		t.seekg(0, std::ios::end);
		int size = t.tellg();
		//struct stat fileStat;
		//int err = stat(path.data(), &fileStat);
		//Log("Size of file:%d | %d", size, fileStat.st_size);
		buffer.resize(size);
		t.seekg(0);
		t.read(buffer.data(), buffer.size());
	}
	Helper::replaceAll(buffer, "\\n", "\n");
	return buffer;
}
std::vector<std::pair<uint32_t, uint32_t>> Helper::split_string(std::string_view str, std::string_view delimiter) {
	std::vector<std::pair<uint32_t, uint32_t>> strings;	std::string::size_type pos = 0, prev = 0;
	while ((pos = str.find(delimiter, prev)) != std::string::npos) {
		strings.push_back({ (uint32_t)prev, (uint32_t)(pos - prev) });	prev = pos + 1;
	}
	strings.push_back({ (uint32_t)prev, (uint32_t)(str.size() - prev - 1) });
	return strings;
}
Mixed Get(const std::string& w, tsl::ordered_map<std::string, std::unique_ptr<Token>>& tokens) {
	auto got = tokens.find(w);
	if (got != tokens.end())
		return got->second.get();
	return w;
}
enum class VariableType {
	NA, String, Int, Variable, UnsignedInt
};
//support defines too, so we can define float as f32

void BuildToken(std::vector<std::pair<SymbolType, Mixed>>& symbols, std::vector<std::unique_ptr<Token>>& tokens, std::vector<Token*>& indices, uint8_t& level, Token*& last, std::string_view str, int pos, int& body_last, std::vector<int>& branches, std::vector<SymbolType>& opened, std::vector<int>& parens) {
	parens.clear();
	bool doingBranch = false;
	if (symbols.front().first == SymbolType::Branch || symbols.front().first == SymbolType::While || symbols.front().first == SymbolType::Switch || symbols.front().first == SymbolType::Else || symbols.front().first == SymbolType::Case || symbols.front().first == SymbolType::Default || symbols.front().first == SymbolType::Enum || symbols.front().first == SymbolType::For)
		doingBranch = true;
	if (branches.size() > 0) {
		level = branches.front();
	}
	if (symbols.size() > 1)
		if (symbols[1].first == SymbolType::ParenLeft)
			symbols.front().first = SymbolType::Function;
	if (symbols.back().first == SymbolType::Colon)
		symbols.front().first = SymbolType::Function;
	if (symbols.front().first == SymbolType::Function && symbols.back().first != SymbolType::ArgsEnd) {
		auto t = std::make_unique<FunctionToken>(symbols);
		t->level = level;
		tokens.push_back(std::move(t));
	} 
	else if (symbols.front().first == SymbolType::Class) {
		auto t = std::make_unique<ClassToken>(symbols);
		t->level = level;
		tokens.push_back(std::move(t));
	}
	else if (symbols.front().first == SymbolType::Enum) {
		auto t = std::make_unique<EnumToken>(symbols);
		t->level = level;
		tokens.push_back(std::move(t));
	}
	else {
		auto t = std::make_unique<VariableToken>(symbols);
		t->level = level;
		tokens.push_back(std::move(t));
	}
	auto token = tokens.back().get();
	int spaces = token->level - last->level;
	if (spaces > 0) {
		indices.push_back(last);
	}
	if (spaces < 0 && opened.size() > 0) {
		//no way this is right, should be level - last->level
		last->symbols.push_back({ SymbolType::EndBranch, "}" });
		opened.pop_back();
	}
	if (doingBranch)
		opened.push_back(SymbolType::Branch);
	else while (spaces++ < 0)
		if (indices.size() > 0)
			indices.pop_back();
	std::string tmp = "";
	if (token->name != "entry1") {
		while (1) {
			auto get = indices.back()->children.find(token->name + tmp);
			if (get != indices.back()->children.end()) {
				tmp += "|";
			}
			else {
				tmp = token->name + tmp;
				break;
			}
		}
	}
	else {
		tmp = "entry1";
		token->needs_compile = true;
		delete indices.back()->children[tmp];
	}
	indices.back()->children[tmp] = token;
	indices.back()->_body += str.substr(body_last, pos - body_last);
	body_last = pos;
	token->parent = indices.back();
	last = indices.back()->children.back().second;
	//if (doingBranch)
		//branches.push_back(level);
	symbols.clear();
	level = 1;
}
bool Helper::splitTokens(std::vector<std::unique_ptr<Token>>& tokens, std::string_view str) {
	std::vector<int> branches;
	std::vector<SymbolType> opened;
	int body_last = 0;
	std::vector<int> parens;
	size_t end_exp = 0;
	//std::string str = Helper::readFile("../parser/gl.fe"); //tmp
	std::string name = "";
	std::vector<std::pair<SymbolType, Mixed>> symbols;
	uint8_t level = 1;
	bool found_first_nonspace = false;
	bool has_colon = false;
	bool doing_array = false;
	SymbolType next = SymbolType::Variable;
	//std::vector<std::unique_ptr<Token>> tokens;
	//std::unique_ptr<ClassToken> root = std::make_unique<ClassToken>();
	//root->name = "root"; root->level = 0;
	//tokens.push_back(std::move(root));
	std::vector<Token*> indices = {tokens.back().get()};
	Token* last = tokens.front().get();
	SymbolType* possible_func = nullptr;
	int in_paren = 0;
	for (uint32_t i = 0; i < str.size(); ++i) {
		char c = str[i];
		if (c == ' ') {
			if (!found_first_nonspace) {
				end_exp = str.find_first_not_of(" ", i);
				auto test = str.substr(i, end_exp - i); //Remove this, its only used to test if this works
				if (test.size() > 0)
					i = end_exp - 1;
				level += test.size();
				found_first_nonspace = true;
			}
			continue;
		} 
		else
			found_first_nonspace = true;
		if (isdigit(c)) {
			end_exp = str.find_first_not_of("0123456789.", i);
			std::string test = str.data(); test = test.substr(i, end_exp - i); //Remove this, its only used to test if this works
			if (int(test.find('.')) > -1) {
				auto d = std::stod(test);
				if (d > FLT_MAX)
					symbols.push_back({ SymbolType::Double, d});
				else
					symbols.push_back({ SymbolType::Float, d });
			}
			else {
				auto t = stoull(test);
				if (t < UINT8_MAX)
					symbols.push_back({ SymbolType::U8, t });
				else if (t < UINT16_MAX)
					symbols.push_back({ SymbolType::U16, t });
				else if (t < UINT32_MAX)
					symbols.push_back({ SymbolType::U32, t });
				else if (t < UINT64_MAX)
					symbols.push_back({ SymbolType::U64, t });
			}
			i += test.size() - 1;
			continue;
		}
		else if (isalpha(c) || c == '_') {
			end_exp = str.find_first_of(" :|()[]-+=!~#@'`\"\r\n,<>*&", i);
		}
		else if (c == '+') {
			symbols.push_back({ SymbolType::Plus, "+" });
			continue;
			//i++;
		}
		else if (c == '#') {
			//#GLOBAL_VAR VAL
			end_exp = str.find_first_of(" ", i);
			std::string var = str.data(); var = var.substr(i+1, end_exp - i-1);
			i += var.size() + 2;
			end_exp = str.find_first_of("\n", i);
			std::string val = str.data(); val = val.substr(i, end_exp - i);
			i += val.size() - 1;
			auto& vm = Parsed::get()->getVM();
			vm.addMacro(var, val);
			//replaceAll(val, "=", to);
		}
		else if (c == '=') {
			if (symbols.front().first != SymbolType::Branch && symbols.front().first != SymbolType::For && symbols.front().first != SymbolType::While && symbols.front().first != SymbolType::Enum && symbols.front().first != SymbolType::Function)
			symbols.front().first = SymbolType::Variable;
			symbols.push_back({ SymbolType::Equals, "=" });
			continue;
		}
		else if (c == '(') {
			int type = 0;
			if (symbols.back().first == SymbolType::TypeOf) {
				//symbols.push_back({ SymbolType::ParenLeft, "(" });
				end_exp = str.find_first_of(')', i + 1);
				std::string test = str.data(); test = test.substr(i + 1, end_exp - i - 1); //Remove this, its only used to test if this works
				symbols.back().second = test;
				i += test.size() + 1;
			}
			else if (symbols.back().first != SymbolType::Branch && symbols.back().first != SymbolType::For && symbols.back().first != SymbolType::While && symbols.back().first != SymbolType::Switch && symbols.back().first != SymbolType::SizeOf) {
				if (symbols.back().first != SymbolType::CastAs && symbols.back().first != SymbolType::TypeOf && symbols.back().first != SymbolType::AngleBracketLeft  && symbols.back().first != SymbolType::ParenLeft && symbols.back().first != SymbolType::ArgsStart) {
					symbols.back().first = SymbolType::Function;
					type = 1;
					symbols.push_back({ SymbolType::ArgsStart, "(" });
				} else
					symbols.push_back({ SymbolType::ParenLeft, "(" });
			}
			if (symbols.back().first == SymbolType::Branch || symbols.back().first == SymbolType::Switch || symbols.back().first == SymbolType::For) {
				symbols.push_back({ SymbolType::ArgsStart, "(" });
				type = 2;
			}
			in_paren++;
			parens.push_back(type);
			continue;
		}
		else if (c == '[') {
			//end_exp = str.find_first_of(']', i + 1);
			if (symbols.back().first == SymbolType::Type) { //this is an array
				doing_array = true;
				if (str[i + 1] == ']') {
					symbols.back().second = "vec$"+std::get<std::string>(symbols.back().second);
					i++;
					continue;
				}
				symbols.push_back({ SymbolType::ArrayStart, "[" });
				/*std::string type_name = std::get<std::string>(symbols.back().second);
				symbols.back().second = type_name + "$";
				if (str[i + 1] == ']')
					i++;
				else
					printf("Array not formed properly.\n");*/
			}
			else if (symbols.back().first == SymbolType::Equals) {
				doing_array = true;
				symbols.push_back({ SymbolType::ArrayStart, "[" });

			}
			else { //this is accessing raw data
				symbols.push_back({ SymbolType::BracketLeft, "[" });
				doing_array = false;
			}
			continue;
		}
		else if (c == ']') {
			if (doing_array) //this is an array
				symbols.push_back({ SymbolType::ArrayEnd, "]" });
			else //this is accessing raw data
				symbols.push_back({ SymbolType::BracketRight, "]" });
			continue;
		}
		else if (c == ')') {
			in_paren--;
			auto v = parens.back();
			parens.pop_back();
			if (v == 1)
				symbols.push_back({ SymbolType::ArgsEnd, ")" });
			else if (v == 2)
				symbols.push_back({ SymbolType::BranchArgsEnd, ")" });
			else
				symbols.push_back({ SymbolType::ParenRight, ")" });
			continue;
		}
		else if (c == ':') {
			if (symbols.back().first == SymbolType::ArgsEnd)
				symbols.front().first = SymbolType::Function;
			else {
				if (symbols.back().first != SymbolType::Type)
					symbols.back().first = SymbolType::Variable;
			}
			next = SymbolType::Type;
			symbols.push_back({ SymbolType::Colon, "" });
			continue;
		}
		else if (c == '"') {
			end_exp = str.find_first_of('"', i + 1);
			std::string test = str.data(); test = test.substr(i + 1, end_exp - i - 1); //Remove this, its only used to test if this works
			symbols.push_back({ SymbolType::String, test });
			i += test.size() + 1;
			continue;
		}
		else if (c == '\'') {
			end_exp = str.find_first_of("\'", i + 1);
			std::string test = str.data(); test = test.substr(i + 1, end_exp - i - 1); //Remove this, its only used to test if this works
			if (test.size() > 1)
				printf("Too large for char.\n");
			symbols.push_back({ SymbolType::I8, (int64_t)test.front() });
			i += test.size() + 1;
			continue;
		}
		else if (c == '\r')
			continue;
		else if (c == '/') {
			if (str[i + 1] == '/') {
				auto next = str.find_first_of("\n", i);
				if (next > str.size())
					break;
				i = next - 1;
				continue;
			}
		}
		else if (c == '\n') {
			if (symbols.size() == 0)
				continue;
			found_first_nonspace = false;
			if (symbols.size() > 0)
				BuildToken(symbols, tokens, indices, level, last, str, i, body_last, branches, opened, parens);
			next = SymbolType::Variable;
			//if (i < str.size() - 1)
			  //branches.clear();
			continue;
		}
		else if (c == '.') {
			end_exp = str.find_first_not_of(".", i);
			std::string var = str.data(); var = var.substr(i, end_exp - i);
			i += var.size() - 1;
			if (var.size() > 3) {
				printf("Too many '.' for parameter pack.");
			}
			else if (var.size() == 3) {
				symbols.push_back({ SymbolType::Type, "variadic" });
				continue;

			}
			else if (var.size() == 2) {
				printf("Too few '.' for parameter pack.");
			}
		}
		else if (c == '<') {
			symbols.back().first = SymbolType::Function;
			if (symbols.back().first == SymbolType::CastAs) {
				end_exp = str.find_first_of('>', i + 1);
				std::string test = str.data(); test = test.substr(i + 1, end_exp - i - 1); //Remove this, its only used to test if this works
				symbols.back().second = test;
				i += test.size() + 1;
				continue;
			}
		}
		else
			end_exp++;
#undef max
		end_exp = std::max(end_exp, (size_t)i);
		std::string test = str.data(); test = test.substr(i, end_exp - i); //Remove this, its only used to test if this works
		if (test.size() == 0)
			test = c;
		auto& vm = Parsed::get()->getVM();
		auto got = vm.reserved_keywords.find(test);
		if (got != vm.reserved_keywords.end()) {
			symbols.push_back({got->second, test });
			if (got->second == SymbolType::Type)
				next = SymbolType::Variable;
		}
		else {
			symbols.push_back({ next, test });
			next = SymbolType::Variable;
		}
		//if (test.size() > 0)
		i += test.size() - 1;
	}
	for (auto o : opened)
		last->symbols.push_back({ SymbolType::EndBranch, "}" });
	opened.clear();
	/*if (branches.size() > 0) {
		int diff = level - branches.back();
		while(diff++ < 0)
			last->symbols.push_back({ SymbolType::EndBranch, "}" });
		if (branches.size() > 0)
			level = branches.back();
	}*/
	if (symbols.size() != 0)
		BuildToken(symbols, tokens, indices, level, last, str, str.size(), body_last, branches, opened, parens);
	branches.clear();
	return false;
}
std::string Helper::GetSpaces(uint16_t level) {
	std::string spaces = "";
	for (int i = 0; i < level; ++i)
		spaces += " ";
	return spaces;
}
//#define FMT_STRING_ALIAS 1
#include <fmt/format.h>

using namespace fmt;
void Helper::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
//slower by 30% than the split_string method but produces string_view
std::vector<std::string> Helper::split(std::string_view strv, std::string_view delims) { std::vector<std::string> output; size_t first = 0; while (first < strv.size()) { const auto second = strv.find_first_of(delims, first); if (first != second) output.emplace_back(strv.substr(first, second - first)); if (second == std::string_view::npos) break; first = second + 1; } return output; }
//slower by 2x than the split_string method but produces strings
std::vector<std::string_view> Helper::splitSV(std::string_view strv, std::string_view delims) { std::vector<std::string_view> output; size_t first = 0; while (first < strv.size()) { const auto second = strv.find_first_of(delims, first); if (first != second) output.emplace_back(strv.substr(first, second - first)); if (second == std::string_view::npos) break; first = second + 1; } return output; }

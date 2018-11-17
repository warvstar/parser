#include "helper.h"
#include "parser.h"
#include "tokens.h"
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
size_t Helper::find_any(std::string_view str, std::vector<std::string_view>& delimiters, size_t prev, size_t end) {
	size_t temp_pos = std::string::npos;
	std::string::size_type pos = 0;
	size_t _min = std::string::npos;
	for (auto delimiter : delimiters) {
		if ((pos = str.find(delimiter, prev)) < end) {
			_min = std::min(_min, pos);
		}
	}
	return _min;
}
std::vector<std::pair<uint32_t, uint32_t>> Helper::split_line(std::string_view str, uint32_t start, uint32_t end, std::vector<std::string_view> delimiters) {
	std::vector<std::pair<uint32_t, uint32_t>> strings;	std::string::size_type pos = start, prev = start;
	size_t end2 = start + end;
	while ((pos = find_any(str, delimiters, prev, end2)) < end2) {
		strings.push_back({ (uint32_t)prev, (uint32_t)(pos - prev) });	prev = pos + 1;
	}
	strings.push_back({ (uint32_t)prev, (uint32_t)(end - start) });
	return strings;
}
// some_int : int = 1
//change this to use string_view
/*void Helper::parse(UnparsedToken& token, SettingWhat& what, std::string_view str) {
	/*if (str.size() > 0) {
		if (token.values[0].size() == 0) {
			if (str == "class") {
				token.tokenType = CLASS;
				token.values[0] = str;
				what = TYPE;
			}
			else {
				token.values[0] = str;
				if (token.tokenType != FUNCTION)
					token.tokenType = VARIABLE;
			}
		}
		else if (token.values[1] == "" && what == TYPE) {
			token.values[1] = str;
		}
		else {
			if (str != "extends")
				token.values.push_back(str.data());
		}
	}*/
//}
std::string Helper::toLua(std::string_view str) {
	std::string s = "";

	return s;
}
/*void getArgs(UnparsedToken& token, std::string_view str, uint32_t& i, uint32_t end) {
	std::string args = "(";
	i++;
	size_t start = i;
	size_t end2 = start + end;
	int inside_breaket = 0;
	for (i = start; i < end2; ++i) {
		args += str[i];
		if (str[i] == '(')
			inside_breaket++;
		else if (str[i] == ')')
			if (inside_breaket == 0)
				return;
			else
				inside_breaket--;
	}
}
void Helper::parseArgs(UnparsedToken& token, std::string_view str, uint32_t& i, uint32_t end) {
	bool inside_string = false;
	bool inside_args = false;
	std::string _string = "";
	i++;
	size_t start = i;
	size_t end2 = start + end;
	std::string ntv[3] = { "", "", "" };
	SettingWhat settingWhat = NAME;
	std::string* last = nullptr;
	int has_b_error = 1;
	token.args.push_back({ "(", ntv[1], ntv[2] });
	for (i = start; i < end2; ++i) {
		if (inside_string) {
			if (str[i] == '"') {
				if (str[i - 1] != '\\') {
					_string += "\"";
					inside_string = !inside_string;
					if (ntv[0].size() == 0)
						ntv[0] = _string;
					else if (ntv[1].size() == 0)
						ntv[1] = _string;
					else
						ntv[2] = _string;
					token.args.push_back({ ntv[0], ntv[1], ntv[2] });
					ntv[0] = "";
					ntv[1] = "";
					ntv[2] = "";
					_string = "";
					continue;
				}
			}
			_string += str[i];
		}
		else {
			switch (str[i]) {
			case ' ':
				break;
			case ',':
				if (_string.size() > 0) {
					if (ntv[0].size() == 0)
						ntv[0] = _string;
					else if (ntv[1].size() == 0)
						ntv[1] = _string;
					else
						ntv[2] = _string;
					token.args.push_back({ ntv[0], ntv[1], ntv[2] });
				}
				token.args.push_back({ ",", ntv[1], ntv[2] });
				ntv[0] = "";
				ntv[1] = "";
				ntv[2] = "";
				_string = "";
				break;
			case '"':
				if (str[i] == '"') {
					if (str[i - 1] != '\\') {
						inside_string = !inside_string;
						if (_string.size() > 0) {
							if (ntv[0].size() == 0)
								ntv[0] = _string;
							else if (ntv[1].size() == 0)
								ntv[1] = _string;
							else
								ntv[2] = _string;
							token.args.push_back({ ntv[0], ntv[1], ntv[2] });
						}
						_string = str[i];
						continue;
					}
				}
				break;
			case ':':
				if (has_b_error == 0) {
					i--;
					return;
				}
				ntv[0] = _string;
				_string = "";
				break;
			case '=':
				ntv[1] = _string;
				_string = "";
				break;
			case '(':
				has_b_error++;
				if (_string.size() > 0) {
					if (ntv[0].size() == 0)
						ntv[0] = _string;
					else if (ntv[1].size() == 0)
						ntv[1] = _string;
					else
						ntv[2] = _string;
					token.args.push_back({ ntv[0], ntv[1], ntv[2] });
				}
				token.args.push_back({ "(", ntv[1], ntv[2] });
				ntv[0] = "";
				ntv[1] = "";
				ntv[2] = "";
				_string = "";
				break;
			case ')':
				has_b_error--;
				if (_string.size() > 0) {
					if (ntv[0].size() == 0)
						ntv[0] = _string;
					else if (ntv[1].size() == 0)
						ntv[1] = _string;
					else
						ntv[2] = _string;
					token.args.push_back({ ntv[0], ntv[1], ntv[2] });
				}
				token.args.push_back({ ")", ntv[1], ntv[2] });
				if (has_b_error == 0)
					return;
				ntv[0] = "";
				ntv[1] = "";
				ntv[2] = "";
				_string = "";
				break;
			default:
				_string += str[i];
				break;
			}
		}
	}

}*/
std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*> Get(const std::string& w, tsl::ordered_map<std::string, std::unique_ptr<Token>>& tokens) {
	auto got = tokens.find(w);
	if (got != tokens.end())
		return got->second.get();
	return w;
}
void FindVarOrFunc(tsl::ordered_map<std::string, std::unique_ptr<Token>>& tokens, std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& tokens2, std::string_view str, uint32_t& start, uint32_t end) {
	std::string _string = "";
	start++;
	for (uint32_t i = start; i < end; ++i) {
		switch (str[i]) {
		case ':':
			tokens2.push_back({ SymbolType::Function, Get(_string, tokens) });
			return;
		case '=':
			tokens2.push_back({ SymbolType::Variable, Get(_string, tokens) });
			return;
		case ' ':
			break;
		default:
			_string += str[i];
		}
	}
}
enum class VariableType {
	NA, String, Int, Variable, UnsignedInt
};
void push_var(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols, VariableType type, const std::string& _string) {
	if (type == VariableType::String)
		symbols.push_back({ SymbolType::String, _string });
	else if (type == VariableType::Int)
		symbols.push_back({ SymbolType::Int, stoll(_string) });
	else if (type == VariableType::UnsignedInt)
		symbols.push_back({ SymbolType::UnsignedInt, stoull(_string) });
	else
		symbols.push_back({ SymbolType::Variable, _string });
}
bool Helper::splitTokens(std::vector<std::unique_ptr<Token>>& tokens, std::string_view str, uint32_t& start, uint32_t end) {
	bool found_first_non_space = false;
	size_t end2 = start + end + 1;
	std::string _string = "";
	bool inside_string = false;
	std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>> symbols;
	std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>> token;
	//global_handle:i64 = 1 + 1
	//entry:void:
	int temp = -1;
	Token* last = nullptr;
	Token* current = nullptr;
	SymbolType doing = SymbolType::NA;
	bool closingFunc = false;
	bool has_close_brace = false;
	bool possibly_function = false;
	int level = 1;
	VariableType type = VariableType::NA;
	for (uint32_t i = start; i < end2; ++i) {
		if (inside_string) {
			if (str[i] == '"') {
				if (str[i - 1] != '\\') {
					_string += "\"";
					symbols.push_back({ SymbolType::String, _string });
					inside_string = !inside_string;
					_string = "";
					continue;
				}
			}
			_string += str[i];
			//or

		}
		else {
			switch (str[i]) {
			case '!':
				symbols.push_back({ SymbolType::Function, _string });
				_string.clear();
				doing = SymbolType::Function;
				break;
			case '(':
				/*tokens[_string] = std::make_unique<FunctionToken>();
				tokens[_string]->symbols.push_back({ SymbolType::BracketLeft, SymbolType::BracketLeft });
				last = current;
				current = tokens[_string].get();
				last->children[_string] = current;
				*/
				symbols.push_back({ SymbolType::Function, _string });
				_string.clear();
				doing = SymbolType::Function;
				break;
			case ')':
				if (doing == SymbolType::Variable) {
					symbols.push_back({ SymbolType::Type, _string });
				}
				else if (doing == SymbolType::Comma) {
					push_var(symbols, type, _string);
				}
				else if (doing == SymbolType::Type) {
					symbols.push_back({ SymbolType::Type, _string });
				}
				else {
					push_var(symbols, type, _string);
				}
				has_close_brace = true;
				_string.clear();
				symbols.push_back({ SymbolType::ArgsEnd, _string });
				doing = SymbolType::BracketRight;
				closingFunc = true;
				break;
			case ',':
				if (doing == SymbolType::Type) {
					symbols.push_back({ SymbolType::Type, _string });
				}
				else {
					push_var(symbols, type, _string);
				}
				_string.clear();
				doing = SymbolType::Comma;
				break;
			case '+':
				if (_string.size() > 0) {
					push_var(symbols, type, _string);
					_string.clear();
				}
				symbols.push_back({ SymbolType::Plus, SymbolType::Plus });
				doing = SymbolType::Plus;
				break;
			case '=':
				/*tokens[_string] = std::make_unique<VariableToken>();
				tokens[_string]->symbols.push_back({ SymbolType::Equals, SymbolType::Equals });
				last = current;
				current = tokens[_string].get();
				last->children[_string] = current;
				*/
				if (doing == SymbolType::Variable)
					symbols.push_back({ SymbolType::Type, _string });
				else
					push_var(symbols, type, _string);

				_string.clear();
				doing = SymbolType::Variable;
				//doing = SymbolType::Equals;
				type = VariableType::NA;
				break;
			case ':':
				if (doing == SymbolType::NA) {
					if (temp == -1) {
						possibly_function = true;
						push_var(symbols, type, _string);
						temp = symbols.size() - 1;
						_string.clear();
						doing = SymbolType::Variable;
					}
					else {
						symbols[temp].first = SymbolType::Function;
						push_var(symbols, type, _string);
						_string.clear();
						doing = SymbolType::Function;
					}
				}
				else if (doing == SymbolType::Function) {
					doing = SymbolType::Type;
					push_var(symbols, type, _string);
					temp = symbols.size() - 1;
					_string.clear();
				}
				else if (doing == SymbolType::Comma){
					doing = SymbolType::Type;
					push_var(symbols, type, _string);
					temp = symbols.size() - 1;
					_string.clear();
				}
				else if (doing == SymbolType::Type) {
					if (_string != "") {
						symbols.push_back({ SymbolType::Type, _string });
						_string.clear();
					}
					if (closingFunc) {
						//turn symbols into a token type Function
						auto t = std::make_unique<FunctionToken>(symbols);
						t->level = level;
						tokens.push_back(std::move(t));
						return true;
					}
				}
				else if (doing == SymbolType::BracketRight) {
					closingFunc = true;
					doing = SymbolType::Type;
				}
				break;
			case ' ':
				if (!found_first_non_space)
					level++;
				if (_string == "class") { //todo: do something different, this is not effecient to cehck every space
					_string.clear();
					doing = SymbolType::Class;
				}
				else if (doing == SymbolType::Class) {
					symbols.push_back({ SymbolType::Class, _string });
					doing = SymbolType::Extends;
					_string.clear();
				} 
				else if (_string == "extends") { //todo: do something different, this is not effecient to cehck every space
					_string.clear();
					doing = SymbolType::Extends;
				}
				else if (doing == SymbolType::Class) {
					symbols.push_back({ SymbolType::Extends, _string });
					doing = SymbolType::NA;
					_string.clear();
				}
				if (type == VariableType::Int || type == VariableType::UnsignedInt) {
					push_var(symbols, type, _string);
					_string.clear();
				}

				type = VariableType::NA;
				break;
			case '\n':
				//turn symbols into a token type
				goto finish;
				break;
			case '\r':
				break;
			case '/':
				if (str[i + 1] == '/') {
					int ii = str.find_first_of('\n', i);
					if (ii > 0)
						start = ii;
					//everything else on this line is a comment
					goto finish;
				}
				break;
			case '"':
				if (type == VariableType::NA)
					type = VariableType::String;
				break;
			case '-':
				doing = SymbolType::Minus;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (type == VariableType::NA)
					if (doing == SymbolType::Minus)
						type = VariableType::Int;
					else
						type = VariableType::UnsignedInt;
			default:
				possibly_function = false;
				if (type == VariableType::NA)
					type = VariableType::Variable;
				found_first_non_space = true;
				_string += str[i];

			}
		}
	}
finish:;
	if (possibly_function) {
		auto t = std::make_unique<FunctionToken>(symbols);
		t->level = level;
		tokens.push_back(std::move(t));
		return true;
	}
	else if (doing == SymbolType::Plus) {
		push_var(symbols, type, _string);
		_string.clear();
		if (symbols.front().first == SymbolType::Variable || symbols.front().first == SymbolType::Int || symbols.front().first == SymbolType::UnsignedInt) {
			auto t = std::make_unique<VariableToken>(symbols);
			t->level = level;
			tokens.push_back(std::move(t));
			return true;
		}
	}
	else {
		if (doing == SymbolType::Variable)
			push_var(symbols, type, _string);
		else if (doing == SymbolType::Class) {
			symbols.push_back({ SymbolType::Class, _string });
			doing = SymbolType::Extends;
			_string.clear();
			auto t = std::make_unique<ClassToken>(symbols);
			t->level = level;
			tokens.push_back(std::move(t));
			return true;
		}
		_string.clear();
		if (symbols.size() == 0)
			return false;
		if (symbols.front().first == SymbolType::Variable || symbols.front().first == SymbolType::Int || symbols.front().first == SymbolType::UnsignedInt) {
			auto t = std::make_unique<VariableToken>(symbols);
			t->level = level;
			tokens.push_back(std::move(t));
			return true;
		}
	}
	return false;
}
#if 0
void Helper::splitTokens(UnparsedToken& token, std::string_view str, uint32_t& start, uint32_t end) {
	bool found_first_non_space = false;
	bool inside_string = false;
	bool inside_args = false;
	bool foundSpace = false;
	bool possibleName = false;
	bool possibleVariable = true;
	bool possibleFunction = false;
	std::string _string = "";
	size_t end2 = start + end + 1;
	SettingWhat settingWhat = NAME;
	for (uint32_t i = start; i < end2; ++i) {
		if (inside_string) {
			if (str[i] == '"') {
				if (str[i - 1] != '\\') {
					_string += "\"";
					if (possibleName)
						parse(token, settingWhat, _string);
					else
						token.values.push_back(_string);
					inside_string = !inside_string;
					_string = "";
					continue;
				}
			}
			_string += str[i];
			//or

		}
		else {
			switch (str[i]) {
			case '(':
				possibleFunction = true;
				parseArgs(token, str, i, end - (i - start));
				break;
			case '!':
				possibleFunction = true;
				_string += str[i];
				break;
			case '/':
				if (str[i + 1] == '/') {
					int ii = str.find_first_of('\n', i);
					if (ii > 0)
						start = ii;
					//everything else on this line is a comment
					goto finish;
				}
				break;
			case ' ':
				if (!found_first_non_space)
					token.starting_spaces++;
				else if (_string.size() > 0) {
					foundSpace = true;
					//parse(token, settingWhat, _string);
					//_string = "";
				}
				break;
			case '"':
				if (str[i] == '"') {
					if (str[i - 1] != '\\') {
						inside_string = !inside_string;
						if (_string.size() > 0) {
							if (possibleName)
								parse(token, settingWhat, _string);
							else
								token.values.push_back(_string);
						}
						_string = "\"";
						continue;
					}
				}
				break;
			case ':':
				if (possibleFunction)
					token.tokenType = FUNCTION;
				possibleFunction = true;
				possibleName = true;
				parse(token, settingWhat, _string);
				_string = "";
				if (settingWhat == TYPE) { //This is a function def, parse the rest seperately
					possibleVariable = false;
					token.tokenType = FUNCTION;
					token.children.emplace_back(UnparsedToken());
					i++;
					splitTokens(token.children.back(), str, i, end - (i - 1 - start) - 1);
					if (token.children.back().tokenType == NA)
						token.children.pop_back();
					goto finish;
				}
				settingWhat = TYPE;
				break;
			case '=':
				possibleName = true;
				parse(token, settingWhat, _string);
				settingWhat = VALUE;
				_string = "";
				token.values.push_back("=");
				//_string += str[i];
				break;
			case '\r':
				if (_string.size() > 0) {
					if (possibleName)
						parse(token, settingWhat, _string);
					else
						token.values.push_back(_string);
				}
				_string = "";
				break;
			case '\n':
				if (_string.size() > 0) {
					if (possibleName)
						parse(token, settingWhat, _string);
					else
						token.values.push_back(_string);
				}
				_string = "";
				break;
			default:
				if (foundSpace) {
					if (possibleName)
						parse(token, settingWhat, _string);
					else {
						token.values.push_back(_string);
						if (_string.size() == 5)
							if (_string == "class")
								token.tokenType = CLASS;
					}
					foundSpace = false;
					_string = "";
				}
				found_first_non_space = true;
				_string += str[i];
				break;
			}
		}
	}
finish:;
	if (possibleFunction && token.tokenType != FUNCTION) {
		if (_string.size() > 0)
			token.values.push_back(_string);
		for (auto arg : token.args) {
			token.values.push_back(arg.name);
		}
		token.tokenType = VARIABLE;
	}
	if (token.tokenType == NA && token.values.size() > 2)
		token.tokenType = VARIABLE;
}
#endif
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
void fill_args(std::string& out, std::vector<std::string>& data, std::pair<std::string, int> to_fill, uint16_t& start) {
	std::unordered_map<std::string, FunctionToken*>::iterator got;
	std::unordered_map<std::string, VariableToken*>::iterator got_var;
	start++;
	bool combine_next = false;
	std::string temp = "";
	std::vector<std::string> temp_vec;
	for (uint16_t i = start; i < data.size(); ++i) {
		auto v = data[i];
		if (v == "+" || v == "-" || v == "=") {

			temp += v;
			combine_next = true;
		}
		else {
			auto g = Parsed::get();
			got = g->functions.find(v);
			if (got != Parsed::get()->functions.end()) {
				temp += "{}";
				std::string t = v + "(";
				int8_t args_need = 0;
				bool has_args = false;
				for (auto a : got->second->args) {
					args_need++;
					t += "{},";
					has_args = true;
				}
				if (has_args)
					t.pop_back();
				t += ")";
				fill_args(out, data, { t, args_need }, i);
			}
			else if (combine_next) {
				temp += v;
				combine_next = false;
				temp_vec.push_back(temp);
			}
			else {
				temp += v;
				temp_vec.push_back(temp);
			}
		}
	}
}
//slower by 30% than the split_string method but produces string_view
std::vector<std::string> Helper::split(std::string_view strv, std::string_view delims) { std::vector<std::string> output; size_t first = 0; while (first < strv.size()) { const auto second = strv.find_first_of(delims, first); if (first != second) output.emplace_back(strv.substr(first, second - first)); if (second == std::string_view::npos) break; first = second + 1; } return output; }
//slower by 2x than the split_string method but produces strings
std::vector<std::string_view> Helper::splitSV(std::string_view strv, std::string_view delims) { std::vector<std::string_view> output; size_t first = 0; while (first < strv.size()) { const auto second = strv.find_first_of(delims, first); if (first != second) output.emplace_back(strv.substr(first, second - first)); if (second == std::string_view::npos) break; first = second + 1; } return output; }

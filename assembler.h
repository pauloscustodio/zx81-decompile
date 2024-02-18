#pragma once

#include <bitset>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class Assembler {
public:
	void assemble(int pass, const string& text);
	
private:
	unordered_map<string, int> m_symbols;
	vector<uint8_t> m_bytes;
	bitset<1> m_used_bytes;
	int m_pc{0};
	
	// parser
	const char* p{nullptr};
	void skip_spaces();
	bool parse_ident(string& ident);
	bool parse_integer(int& value);
	bool match(const string& text);
};

Assembler assembler;

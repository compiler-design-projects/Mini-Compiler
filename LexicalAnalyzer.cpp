/*Melissa Myers
Program #3
March 8, 2016
Breaks down source code into tokens and builds a symbol table containing all reserved words, and names of constants and variables.
Then proceeds to group the tokens into meaningful clusters (parsing declarations).

LexicalAnalyzer.cpp : Defines the entry point for the console application. */


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <string>


using namespace std;

ifstream in;
ofstream out;
ofstream out2;
ofstream newtable;
ifstream in2;

const int tablesize = 50;	//symbol table size
const int ST = 32;			//string
const int INT = 101;		//int
const int INTCON = 102;		//int constant
const int INTV = 103;		//int variable
const int CH = 107;			//character					
const int CHCON = 108;		//character constant
const int CHVAR = 109;		//character variable
const int ARTY = 110;		//array type
const int RES = 111;		//reserved words 
const int OP = 112;			//operators
const int PUN = 113;		//punctuation			
const int NE = 200;			//!=
const int LE = 201;			//<=
const int GE = 202;			//>=
const int EE = 203;			// ==
const int LOGOR = 204;		//||
const int LOGAND = 205;		//&&
const int IN = 206;			//>>
const int OUT = 207;		//<<
const int INCOP = 208;		//++
const int DECOP = 209;		//--
const int ASG = 61;			//=
const int TS = 5;			//temp size

char token[TS];
int atype;
int avalue;
int toktype;
int eletype;
int charClass = 0;
int lineNumber = 0;
int position = 0;
int arrsize;
char ch;
int mainflag = 0;
int semiflag = 0;
int tempnum = 0; 
string lexeme;
string astring;
string comment;
std::pair<int, int> ctoken;
std::pair<int, int> oldtoken;

std::vector<std::string> reswords = { "","int", "char", "const", "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char16_t", "char32_t","cin","class", "compl", "concept", "constexpr", "const_cast", "continue", "cout", "decltype", "default", "define", "defined","delete", "do", "double","elif", "else", "endif", "endl",  "enum","error","explicit", "export","extern","false","final","float","for","friend", "goto", "if" "ifdef","ifndef", "include", "iostream", "inline","line","long","mutable","main","namespace","new", "noexcept","not","not_eq","nullptr","operator","or","or_eq","pragma","private","protected","public","register","requires","return","short","signed",
"sizeof","static","static_cast","std","struct","switch", "template","this","throw","true","try","typedef","typeid","typename","undef","union","unsigned", "using","virtual","void","volatile","wchar_t","while","xor","xor_eq" };
int hashfun(char first, int tablesize);
void getChar();
void getToken();
void classifyChar();
bool checkTable(string word);
void error(string s);
void consumeToken();
void parseExp();
void assignment();
void exp(string result);
void exp(char *result);
void term(char *result);
void factor(char* result);
void newtemp(char *result);
//------------------------------------------------class tableEntry-----------------------------------------
class tableEntry {
	friend class SymbolTable;
	//public: void print();

	string name;
	int type;
	int value;
	int size;
	int elType;
public:
	tableEntry(string name, int type, int value, int size, int elType) {
		this->name = name;
		this->type = type;
		this->value = value;
		this->size = size;
		this->elType = elType;
	}

	void print() {
		out.width(10);
		out << left;
		out << name << "\t" << type << "\t" << value << "\t" << size << "\t" << elType << endl;
	}
	void print2() {
		newtable.width(10);
		newtable << left;
		newtable << name << "\t" << type << "\t" << value << "\t" << size << "\t" << elType << endl;
	}

	string getName() {
		return name;
	}
	int getType() {
		return type;
	}

	int getValue() {
		return value;
	}
	int getSize() {
		return size;
	}

	void setType(int newtype) {
		type = newtype;
	}

	void setValue(int newvalue) {
		value = newvalue;
	}
	void setSize(int newsize) {
		size = newsize;
	}
	void setEltype(int neweltype) {
		elType = neweltype;
	}

};
//------------------------------------------------class SymbolTable----------------------------------------
class SymbolTable {
	friend void lex(SymbolTable *s);
	friend void parse(SymbolTable *s);
	friend void isAssignment(SymbolTable *s);
	friend void isArray(SymbolTable *s);
	vector<tableEntry> symbolTable[50];
	friend std::ostream& operator<<(std::ostream& os, const SymbolTable& symbolTable);

public: SymbolTable() {
	vector<tableEntry> symbolTable[50] = {};
}
		void push_back(int hashnum, tableEntry *e) {
			symbolTable[hashnum].push_back(*e);

		}
		bool is_empty(int i) {
			if (symbolTable[i].empty()) {
				return true;
			}
			else return false;
		}
		int size(int hashnum) {
			return symbolTable[hashnum].size();
		}

		bool checkTable(string word) {
			position = 0;
			int hashvalue = hashfun(word.at(0), tablesize);
			vector<tableEntry> ::iterator vec = symbolTable[hashvalue].begin();
			while (vec != symbolTable[hashvalue].end()) {
				if (vec->getName() == word) {

					return false;
				}
				position++;
				vec++;

			}
			return true;


		}

		void print() {
			out << "hashVal\t  " << "name\t      " << "type    " << "value    " << "size    " << "elType\t" << endl;
			for (int i = 0; i < tablesize; i++) {
				if (!symbolTable[i].empty()) {
					out << "   " << i << ":\t";
					int flag = 0;
					vector<tableEntry>::iterator v = symbolTable[i].begin();
					while (v != symbolTable[i].end()) {
						if (flag != 0) {
							out << "  \t";
						}
						v->print();
						flag = 1;
						v++;
					}
				}
			}
		}
		void printupdates() {
			newtable << "hashVal\t  " << "name\t      " << "type    " << "value    " << "size    " << "elType\t" << endl;
			for (int i = 0; i < tablesize; i++) {
				if (!symbolTable[i].empty()) {
					newtable << "   " << i << ":\t";
					int flag = 0;
					vector<tableEntry>::iterator vit = symbolTable[i].begin();
					while (vit != symbolTable[i].end()) {
						if (flag != 0) {
							newtable << "  \t";
						}
						vit->print2();
						flag = 1;
						vit++;
					}
				}
			}
		}

		void updateTable(int index, int pos, int type, int value) {
			vector<tableEntry>::iterator tor = symbolTable[index].begin();
			while (tor != symbolTable[index].end()) {
				if ((tor->getValue() == pos) && (tor->getType() == index)) {
					tor->setType(type);
					tor->setValue(value);
				}
				tor++;
			}
		}

		bool isDeclared(int type, int value) {
			vector<tableEntry>::iterator tor = symbolTable[type].begin();
			while (tor != symbolTable[type].end()) {
				if ((tor->getValue() == value) && (tor->getType() == type)) {
					return false;
				}
				tor++;
			}
			return true;
		}

		bool isConstDeclared(int idtype, int idvalue) {
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[idtype].begin();
			while (tor != symbolTable[idtype].end()) {
				if (count == idvalue && (tor->getType() == CHCON || tor->getType() == INTCON)) {
					return true;
				}
				count++;
				tor++;
			}
			return false;
		}

		int getValue(int type, int value) {
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[type].begin();
			while (tor != symbolTable[type].end()) {
				if (count == value) {
					return tor->getValue();
				}
				count++;
				tor++;
			}
		}

		string getName(int type, int value) {		//convert int to string 
			if (type == CH || type == INT) {
				std::stringstream ss;
				ss << value;
				string name = ss.str();
				return name;
			}
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[type].begin();
			while (tor != symbolTable[type].end()) {
				if (count == value) {
					return tor->getName();
				}
				count++;
				tor++;
			}
		}

		bool isArray(int type, int value) {
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[type].begin();
			while (tor != symbolTable[type].end()) {
				if (count == value) {
					if (tor->getType() == ARTY) {
						return true;
					}

				}
				count++;
				tor++;
			}
			return false;
		}

		int arGetIndex(int type, int value) {
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[type].begin();
			while (tor != symbolTable[type].end()) {
				if (count == value) {
					return tor->getSize();

				}
				count++;
				tor++;
			}
		}


		void secondUpdate(int idtype, int idvalue, int value) {
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[idtype].begin();
			while (tor != symbolTable[idtype].end()) {
				if (count == idvalue) {
					tor->setValue(value);
					tor->setType(toktype);
				}
				count++;
				tor++;
			}
		}

		void updateArr(int idtype, int idvalue, int value, int size, int eltype) {
			int count = 0;
			vector<tableEntry>::iterator tor = symbolTable[idtype].begin();
			while (tor != symbolTable[idtype].end()) {
				if (count == idvalue) {
					tor->setValue(0);
					tor->setSize(size);
					tor->setEltype(eltype);
					tor->setType(toktype);
				}
				count++;
				tor++;
			}
		}

};
SymbolTable *s = new SymbolTable();
//------------------------------------------------MAIN-----------------------------------------------------
int main() {
	//SymbolTable *s = new SymbolTable();
	//------------------------------------------------files----------------------------------------------------
	out.open("outfile.txt", ios::out);
	newtable.open("updatedtable.txt", ios::out);
	out2.open("outfile2.txt", ios::out);
	in.open("infile.txt", ios::in);
	in2.open("outfile2.txt", ios::in);


	for (int i = 1; i < 99; i++) {
		string word = reswords[i];
		int hashnum = hashfun(word.at(0), tablesize);
		tableEntry *e = new tableEntry(reswords[i], RES, i, 0, 0);
		s->push_back(hashnum, e);
	}


	while (!in.eof()) {
		lex(s);
	}
	s->print();
	in.close();
	out.close();
	out2.close();

	cout << endl;
	cout << endl;
	while (!in2.eof() && mainflag == 0 && semiflag == 0) {
		parse(s);
	}
	s->printupdates();
	if (mainflag == 1) {
		parseExp();
	}
	return 0;

}
//------------------------------------------------lex methods----------------------------------------------
int hashfun(char first, int tablesize) {

	return first % tablesize;
}

void addChar() {
	lexeme += ch;
}

void classifyChar() {

	if (ch == ' ') { charClass = 0; }

	if ((ch > 64 && ch < 91) || (ch > 96 && ch < 123) || ch == '_')
	{
		charClass = 1;
	}

	if ((ch > 47 && ch < 58)) {
		charClass = 2;
	}

	if (ch == ';' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == ',' || ch == '#') {
		charClass = 3;
	}

	if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
		charClass = 4;
	}

	if (ch == '>' || ch == '<' || ch == '=' || ch == '!' || ch == '&' || ch == '|') {
		charClass = 5;
	}

	if ((int)ch == 39 || ch == '"') {
		charClass = 6;
	}
	if (ch == '$' || ch == '@') {
		charClass = 7;
	}

}

void getChar() {
	charClass = 0;

	if (ch == '\n') {
		lineNumber++;
	}
	if (!in.eof()) {
		in.get(ch);
		classifyChar();
	}
	if (in.eof()) { charClass = 0; }


}

void getToken() {
	int ty = 0;
	int va = 0;

	in2 >> ty;
	in2 >> va;

	ctoken.first = ty;
	ctoken.second = va;

	if ((ctoken.first == 112 && ctoken.second == 61) || (ctoken.first == 113) && (ctoken.second == 91)) {  //if '=' or '[', we want to save the old token
		return;
	}
	else {
		oldtoken = ctoken;
	}

}

void printToken() {
	cout << ctoken.first << "\t" << ctoken.second << endl;
}
//------------------------------------------------parse methods--------------------------------------------
bool identifier(SymbolTable *s) {										//determines if valid identifier

	getToken();
	if (ctoken.first == 111) {
		if (ctoken.second != 56) {
			cout << "error! this is a reserved word" << endl;
		}
		return 0;
	}
	else if (s->isDeclared(ctoken.first, ctoken.second)) {
		cout << "error! already declared!" << endl;
		return 0;
	}
	else if (ctoken.first > 49) {
		cout << "error! identifier expected!" << endl;
		return 0;
	}
	else if (ctoken.first == 0 && ctoken.second == 0)
	{
		return 0;
	}
	s->updateTable(ctoken.first, ctoken.second, toktype, 0);

	return 1;
}

void identifierlist(SymbolTable *s) {
	getToken();
	if (ctoken.first == 112 && ctoken.second == 61) {			//if '=' 
		atype = oldtoken.first;
		avalue = oldtoken.second;
		isAssignment(s);     									 //token is '='
																 //it's an assignment 
		return;
	}
	else if (ctoken.first == 113 && ctoken.second == 91) {		//this is an array...token is '['
		isArray(s);
		//parse(s);
	}

	else if (ctoken.first == 113 && ctoken.second == 44) {       //this is a comma 
		if (identifier(s)) {									//check for another identifier
			identifierlist(s);									//recurse to check for another comma
		}
	}
	else if (ctoken.first == 113 && ctoken.second == 59) {		//if semicolon
		parse(s);
	}

	else cout << "error in list!" << endl; {					//else, missing comma or missing semicolon
		cout << endl;
		semiflag = 1;
		//break;
	}

}

void isArray(SymbolTable *s) {
	eletype = toktype;										//set element type
	toktype = ARTY;											//set array type
	atype = oldtoken.first; avalue = oldtoken.second;		//keep track of identifier
	getToken();
	if (ctoken.first == INT) {								//arrays can only be sized with ints for now
		arrsize = ctoken.second;
		s->updateArr(atype, avalue, 0, arrsize, eletype);
		getToken();
		if (!(ctoken.second == 93 && ctoken.first == 113)) {			//array does not have closing bracket; 
			cout << "error! needs closing bracket!" << endl;
			semiflag = 1;
			parse(s);
		}
		getToken();
		printToken();
		if (ctoken.first == 113 && ctoken.second == 44) {			//if comma, go back into list
			toktype = eletype;
			if (identifier(s)) {
				identifierlist(s);
			}
		}
		else if (ctoken.first == 113 && ctoken.second == 59) {
			parse(s);

		}
		else {
			cout << "error! needs semicolon!" << endl;
			semiflag = 1;

		}
	}
}

void isAssignment(SymbolTable *s) {
	getToken();
	if (s->isDeclared(atype, avalue)) {						//new variable has been declared
		if (ctoken.first == INT || ctoken.first == CH) {	//if it's a char or an int 
			s->secondUpdate(atype, avalue, ctoken.second);			//simple update
		}
		else if (s->isDeclared(ctoken.first, ctoken.second)) {		//else if it is '= variable' 
			int nvalue = s->getValue(ctoken.first, ctoken.second);	//get variables value from symbol table
			s->secondUpdate(atype, avalue, nvalue);					//update
																	//	s->updateTable();
		}
		else { cout << "error! cannot initialize to a non declared variable!" << endl; }
	}
	else {
		cout << "error! must properly declare variable!" << endl;
	}

	getToken();
	if (ctoken.first == 113 && ctoken.second == 44) {			// if comma
		if (identifier(s)) {
			identifierlist(s);
		}
	}
	else if (ctoken.first == 113 && ctoken.second == 59) {		//if semicolon
		parse(s);

	}
	else {
		cout << "semicolon required!" << endl;			//next token was not ';'
		semiflag = 1;
		//parse(s);

	}


}
//------------------------------------------------lex()----------------------------------------------------
void lex(SymbolTable *s) {
	lexeme = "";
	comment = "";
	astring = "";

	while (charClass == 0 && !in.eof()) {
		getChar();
	}
	switch (charClass) {
	case 1: {
		addChar();
		getChar();
		while (charClass == 1 || charClass == 2) {
			addChar();
			getChar();
		}
		if (std::find(reswords.begin(), reswords.end(), lexeme) != reswords.end()) //it is a reserved word, print it
		{
			cout << RES << "\t" << std::find(reswords.begin(), reswords.end(), lexeme) - reswords.begin() << "\t" << lexeme << endl;
			out2 << RES << "\t" << std::find(reswords.begin(), reswords.end(), lexeme) - reswords.begin() << endl;
		}
		else {							//not reserved word;  needs added to symbol table, then print it

			int hashnum = hashfun(lexeme.at(0), tablesize);
			int pos = s->size(hashnum);
			if (s->checkTable(lexeme)) {
				tableEntry *en = new tableEntry(lexeme, hashnum, pos, 0, 0);
				s->push_back(hashnum, en);
				out2 << hashnum << "\t" << pos << endl;
				cout << hashnum << "\t" << pos << "\t" << lexeme << endl;
			}
			else {
				cout << hashnum << "\t" << position << "\t" << lexeme << endl;
				out2 << hashnum << "\t" << position << "\t" << endl;
			}

		}
		classifyChar();
		break;
	}
	case 2: {
		addChar();
		getChar();
		while (charClass == 2) {		//while digit, keep reading
			addChar();
			getChar();
		}
		cout << INT << "\t" << lexeme << endl;
		out2 << INT << "\t" << lexeme << endl;

		classifyChar();
		break;
	}
	case 3: {
		addChar();
		cout << PUN << "\t" << (int)ch << "\t" << lexeme << endl;
		out2 << PUN << "\t" << (int)ch << "\t" << endl;
		getChar();
		classifyChar();
		break;

	}
	case 4: {
		addChar();
		if (ch == '+' && in.peek() == '+') {
				getChar();
				addChar();
				cout << OP << "\t" << INCOP << "\t" << lexeme << endl;
				out2 << OP << "\t" << INCOP << "\t" << endl;
		}
		else if (ch == '-' && in.peek() == '-') {
			getChar();
			addChar();
			cout << OP << "\t" << DECOP << "\t" << lexeme << endl;
			out2 << OP << "\t" << DECOP << "\t" << endl;
		}
		else if (ch == '/' && in.peek() == '/') {
			while (ch != '\n' || in.eof()) {
				getChar();
			}
		}
		else if (ch == '/' && in.peek() == '*') {
			getChar();
			getChar();
			while (true) {
				if (ch != '*') {
					comment += ch;
					getChar();
				}
				if (ch == '*' && in.peek() == '/') {
					getChar();
					break;

				}
				if (ch == '*' && in.peek() != '/') { getChar(); }
				if (in.eof()) {
					cout << "error! neverending comment!" << endl; break;
				}
			}
		}
		else {
			cout << OP << "\t" << (int)ch << "\t" << lexeme << endl;
			out2 << OP << "\t" << (int)ch << "\t" << endl;
		}
		getChar();

		classifyChar();
		break;

	}
	case 5: {
		addChar();
		if (ch == '&') {
			if (in.peek() == '&') {
				getChar();
				addChar();
				cout << OP << "\t" << LOGAND << "\t" << lexeme << endl;
				out2 << OP << "\t" << LOGAND << "\t" << endl;
			}
			else
				cout << lexeme << " is invalid!" << endl;
		}
		else if (ch == '|') {
			if (in.peek() == '|') {
				getChar();
				addChar();
				cout << OP << "\t" << LOGOR << "\t" << lexeme << endl;
				out2 << OP << "\t" << LOGOR << "\t" << endl;
			}
			else
				cout << lexeme << " is invalid!" << endl;
		}
		else if (ch == '!' && in.peek() == '=') {

			getChar();
			addChar();
			cout << OP << "\t" << NE << "\t" << lexeme << endl;
			out2 << OP << "\t" << NE << "\t" << endl;
		}
		else if (ch == '=' && in.peek() == '=') {
			getChar();
			addChar();
			cout << OP << "\t" << EE << "\t" << lexeme << endl;
			out2 << OP << "\t" << EE << "\t" << endl;
		}
		else if (ch == '<' && in.peek() == '=') {

			getChar();
			addChar();
			cout << OP << "\t" << LE << "\t" << lexeme << endl;
			out2 << OP << "\t" << LE << endl;

		}
		else if (ch == '<' && in.peek() == '<') {
			getChar();
			addChar();
			cout << OP << "\t" << OUT << "\t" << lexeme << endl;
			out2 << OP << "\t" << OUT << endl;

		}

		else if (ch == '>' && in.peek() == '=') {
			getChar();
			addChar();
			cout << OP << "\t" << GE << "\t" << lexeme << endl;
			out2 << OP << "\t" << GE << endl;

		}
		else if (ch == '>' && in.peek() == '>') {
			getChar();
			addChar();
			cout << OP << "\t" << IN << "\t" << lexeme << endl;
			out2 << OP << "\t" << IN << endl;


		}

		else
			cout << OP << "\t" << (int)ch << "\t" << lexeme << endl;
		out2 << OP << "\t" << (int)ch << endl;



		getChar();

		classifyChar();
		break;

	}
	case 6: {
		if ((int)ch == 39) {
			getChar();
			addChar();
			if ((int)in.peek() != 39) {
				cout << "error! unmatched single quote at line " << lineNumber + 1 << endl;
			}
			else {
				cout << CH << "\t" << (int)ch << "\t" << ch << endl;
				out2 << CH << "\t" << (int)ch << endl;
				getChar();

			}
		}
		else if ((int)ch == 34) {
			getChar();
			astring += ch;
			getChar();
			while ((int)ch != 34) {
				if (in.eof()) { break; }
				astring += ch;
				getChar();

			}
			if ((int)ch == 34) {
				getChar();

			}
			else if (in.eof()) {
				cout << "error! unterminated string!" << endl;
				break;
			}
			int hashnum2 = hashfun(astring.at(0), tablesize);
			int pos2 = s->size(hashnum2);
			if (s->checkTable(astring)) {
				tableEntry *ent = new tableEntry(astring, ST, pos2, 0, 0);
				s->push_back(hashnum2, ent);
				cout << ST << "\t" << pos2 << "\t" << astring << endl;
				out2 << ST << "\t" << pos2 << endl;
			}
			else {
				cout << ST << "\t" << position << "\t" << astring << endl;
				out2 << ST << "\t" << position << endl;
			}

		}
		getChar();
		classifyChar();
		break;
	}
	case 7: {  cout << ch << "\t illegal char at line " << lineNumber + 1 << endl;
		getChar();
		classifyChar();
		break;
	}
	}
}
//------------------------------------------------parse()--------------------------------------------------
void parse(SymbolTable *s) {
	while (!in2.eof()) {
		getToken();
		if (ctoken.first == 111 && ctoken.second == 1) { //this is an int type 
			toktype = INTV;
			if (identifier(s) && ctoken.second != 56) {						//if next token is identifier
				identifierlist(s);						//call identifier list 
				break;
			}
			else if (ctoken.first == 111 && ctoken.second == 56)		//'main'

			{
				getToken();
				if (ctoken.first == 113 && ctoken.second == 40) {			//if '('
					cout << "'int main ()' has been reached!" << endl;
					mainflag = 1;
					parse(s);
				}
				else cout << "cannot use main as an identifier";				//no '(' following main
			}
			else {
				cout << "not an identifier!" << endl;
				parse(s);
				break;
			}
		}

		else if (ctoken.first == 111 && ctoken.second == 2) { //this is an char identifier 
			toktype = CHVAR;
			if (identifier(s)) {						//if next token is identifier
				identifierlist(s);						//call identifier list 
				break;
			}

			else {
				cout << "not an identifier!" << endl;
				parse(s);
				break;
			}
		}


		else if (ctoken.first == 111 && ctoken.second == 3) {			//this is an const identifier 
			getToken();
			if (ctoken.first == 111 && ctoken.second == 1) {			//this is a const int
				toktype = INTCON;
			label:
				if (identifier(s)) {									//get the identifier
				}
				getToken();
				if (ctoken.first == 112 && ctoken.second == 61) {		 //if '='
					atype = oldtoken.first; avalue = oldtoken.second;
					getToken();
					if (s->isDeclared(atype, avalue)) {						//new variable has been declared
						if (ctoken.first == INT || ctoken.first == CH) {	//if it's a char or an int 
							s->secondUpdate(atype, avalue, ctoken.second);			//simple update
						}
						else if (s->isConstDeclared(ctoken.first, ctoken.second)) {		//else if it is '= variable' 
							int nvalue = s->getValue(ctoken.first, ctoken.second);	//get variables value from symbol table
							s->secondUpdate(atype, avalue, nvalue);					//update
																					//	s->updateTable();
						}
						else { cout << "error! cannot initialize to a non declared or non constant variable!" << endl; }
					}

					getToken();
					if (ctoken.first == 113 && ctoken.second == 44) {
						goto label;											//if there is a comma, redo
					}
					else if (ctoken.first == 113 && ctoken.second == 59) {		//if semicolon, continue
						parse(s);
					}
					else {
						cout << "error! missing semicolon." << endl;
						semiflag = 1;
					}
				}
				else {
					cout << "error! must define const immediately!" << endl;			//if no '=' followed identifier
				}
				break;
			}


			else if (ctoken.first == 111 && ctoken.second == 2) {		//this is a const char
																		//this is a const int
				toktype = CHCON;
			label2:
				if (identifier(s)) {									//get the identifier
				}
				getToken();
				if (ctoken.first == 112 && ctoken.second == 61) {		 //if '='
					atype = oldtoken.first; avalue = oldtoken.second;
					getToken();
					if (s->isDeclared(atype, avalue)) {						//new variable has been declared
						if (ctoken.first == INT || ctoken.first == CH) {	//if it's a char or an int 
							s->secondUpdate(atype, avalue, ctoken.second);			//simple update
						}
						else if (s->isConstDeclared(ctoken.first, ctoken.second)) {		//else if it is '= variable' 
							int nvalue = s->getValue(ctoken.first, ctoken.second);	//get variables value from symbol table
							s->secondUpdate(atype, avalue, nvalue);					//update
																					//	s->updateTable();
						}
						else { cout << "error! cannot initialize to a non declared or non constant variable!" << endl; }
					}

					getToken();
					if (ctoken.first == 113 && ctoken.second == 44) {
						goto label2;					//if there is a comma, redo 
					}
					else if (ctoken.first == 113 && ctoken.second == 59) {		//if it is semicolon, continue
						parse(s);
					}
					else {
						cout << "error! missing semicolon!" << endl;
						semiflag = 1;
						//parse(s);
					}
				}
				else {
					cout << "error! must define const immediately!" << endl;		//if no '=' followed identifier
				}
				break;



			}
		}
		else if (ctoken.first < 50) {											//plain identifier
			if (s->isConstDeclared(ctoken.first, ctoken.second)) {				//makes sure identifier isn't constant
				cout << "error! const was already initialized!" << endl;
			}
			if (s->isDeclared(ctoken.first, ctoken.second)) {					//checks to make sure identifier was declared properly
				getToken();
				if (ctoken.first == 112 && ctoken.second == 61) {
					atype = oldtoken.first; avalue = oldtoken.second;
					getToken();
					s->secondUpdate(atype, avalue, ctoken.second);
					getToken();
					if (ctoken.first == 113 && ctoken.second == 59) {
						parse(s);
					}
					else {
						cout << "error! missing semicolon!" << endl;
						semiflag = 1;
						//parse(s);
					}
				}

			}
			else cout << "error! must declare identifier first!" << endl;
		}
		else {
			return;	  //this is neither 

		}
	}
}
//-----------------------------------------------------parseExp--------------------------------------------
void error(string s) {
	cout << "Error! " + s << endl;
}
void consumeToken() {
	string name;
	getToken();
	cout << "token : ";
	printToken();
	if (ctoken.first == OP || ctoken.first == PUN) {
		cout << "lets have a chat" << endl;
		switch (ctoken.second) {
		case 40: {
			char temp[TS] = { '(' };
			strcpy(token, temp);
			break;	}
		case 41: {
			char temp[TS] = { ')' };
			strcpy(token, temp);
			break; }
		case 42: {
			char temp[TS] = { '*' };
			strcpy(token, temp);
			break; }
		case 43: {
			char temp[TS] = { '+' };
			strcpy(token, temp);
			break; }
		case 45: {
			char temp[TS] = { '-' };
			strcpy(token, temp);
			break; }
		case 47: {
			char temp[TS] = { '/' };
			strcpy(token, temp);
			break; }
		}
	}
	else {
		name = s->getName(ctoken.first, ctoken.second);
		char const *n = name.c_str();
		strcpy(token, n);

	}
}
void parseExp() {
	getToken();
	if (ctoken.first == 111 && ctoken.second == 25) {
		cout << "this looks like a cout to me!" << endl;
	}
	else if (ctoken.first == 111 && ctoken.second == 18) {
		cout << "this looks like a cin to me!" << endl;
	}
	else {
		assignment();
	}

}

void assignment() {
	char arg[TS]; char index[TS];
	std::pair<int, int> p = ctoken;
	printToken();
	if (p.first == RES) {
		error("A reserved word used in an assignment!");			//if reserved
	}
	if (!s->isDeclared(ctoken.first, ctoken.second)) {			//if undeclared
		error("An undeclared variable in an assignment!");
	}
	if (s->isArray(p.first, p.second)) {				//if array
	//	index = s->arGetIndex(ctoken.first, ctoken.second);
		//cout << "this is an array" << endl;
		//cout << "size: " << index;
	}
	getToken();
	if (!(ctoken.first == OP && ctoken.second == ASG)) {
		error("The assignment operator is missing in an assignment statement.");
	}
	consumeToken(); 
	strcpy(arg, token);
	exp(arg);
	if (s->isArray(p.first, p.second)) {
		cout << "?" << '\t' << arg << '\t' << index << '\t' << s->getName(p.first, p.second);
	}

	else cout << "=" << '\t' << arg << "\t\t" << s->getName(p.first, p.second) << endl;

}

void exp(char *result)
{
	char arg1[TS], arg2[TS];
	strcpy_s(arg1, result);	 			//arg1 = b
	char op;
	cout << "In exp(), current token: " << arg1 << endl;
	term(arg1);				//term(b)
	cout << "Back in exp(), current token: " << token << endl;
	while (token[0] == '-' || token[0] == '+') {
		op = token[0];
		cout << "CONSUMING OPERATOR: " << token << endl;
		consumeToken();
		cout << "next token: " << token; 
		strcpy_s(arg2, token);
		term(arg2);
		newtemp(result);
		cout << "cout from exp " << op << "\t" << arg1 << "\t" << arg2 << "\t" << result << endl;
		strcpy(arg1, result);
	}
	strcpy(result, arg1);
}

void term(char* result)
{
	char arg1[TS], arg2[TS];
	char op;
	strcpy_s(arg1, result);							//arg1 = b
	cout << "In term(), current token: " << arg1 << endl;
	factor(arg1);
	cout << "Back in term(), current token: " << token << endl;	//should be op
	while (token[0] == '*' || token[0] == '/') {			//could totally keep this as token..i did
		op = token[0];								//set op for printing
		cout << "COSUMING OPERATOR: " << token << endl;		//eat da op but it is saved still
		consumeToken();									//next token after op == c
		strcpy_s(arg2, token);
		factor(arg2);								// arg2 = c
		newtemp(result);									//result = t0
		cout << " from term" << op << "\t" << arg1 << "\t" << arg2 << "\t" << result << endl;
		strcpy(arg1, result);
	}
	strcpy(result, arg1);
}

void factor(char *result)
{
	cout << "In factor(), current token: " << token << endl;
	if (result[0] == '(') {					//if arg
		cout << "CONSUMING: " << token << endl;
		consumeToken();
		strcpy(result, token);
		exp(result);
		if (token[0] == ')') {
			//cout << "Error" << endl;
			cout << "CONSUMING: " << token << endl;
			consumeToken();
		}

	}

	else if (isalpha(result[0]) || isdigit(result[0]))		//if b is ID
	{
		cout << "CONSUMING ID: " << token << endl;		//eat b
		if (token[0] != ';') {							//if not semicolon, then you want to get the op
			consumeToken();
		}
	}

}

void newtemp(char *result) {
	strcpy(result, "t");
	std::string s = std::to_string(tempnum);
	char const *pchar = s.c_str();
	strcat(result, pchar);
	tempnum++;
}













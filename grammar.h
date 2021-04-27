#ifndef GRAMMAR_H
#define GRAMMAR_H

#include<iostream>
#include<vector>
#include<string>
#include<iomanip>
#include<fstream>
#include"tokenizer.h"
using namespace std;

/*
	General things concerning implementations of grammar:
		- nextToken() (in conjungtion with getToken()) must succeed all 			terminals
			- look at while_() as an example
		- don't forget to compare lex (not tok) for terminals
		- terminals are enclosed with double quotes " " since
			they are strings
		- Rat18F diction that is present in C++ is appended with
			an underscore
*/

/*
	Assembly Notes
		- all assembly functions are tagged with "// asm"
		- NULL produces warnings, so a constant NIL declared early on
		  in the program is used in its stead
		- added a PUSHI function that wasn't (but should've been) 
		  mentioned in the partial solutions document
		- added assembly functions to primary() instead of factor()
*/

// used in place of NULL in order to avoid warnings during compilation
const int NIL = 0x12345678;

class Grammar {
	protected:
		// current token pointed to by iterator
		Record cToken;
		// switch for printing tokens
		Record tokenSwitch;
		// asm
		Record save;
		vector<int> jumpstack;
		int instr_address,
		    temp_address;
		struct instruction {
			int address;
			string op;
			int oprnd;
		};
		instruction instr_table[1000];	
		int sym_address;
		struct symbol {
			Record token;
			int address;	// add 5000
		};
		vector<symbol> sym_table;
		// for printing assembly instructions and symbol table to file
		ofstream asmFile;
	
	public:
		Grammar();
		// general functions
		void nextToken();
		void printToken();
		void switchT();
		// production rules
		//empty					//R29
		bool primary();				//R28
		bool factor();				//R27
		bool termPrime();			//R26'
		bool term();				//R26
		bool expressionPrime();			//R25'
		bool expression();			//R25
		bool relop();				//R24
		bool condition();			//R23
		bool while_(); //reworded with underscr	//R22
		bool scan();				//R21
		bool print();				//R20
		bool return_();	//underscore		//R19
		bool If();				//R18'
		bool assign();				//R17
		bool compound();			//R16
		bool statementList();			//R14
		bool statement();			//R15
		bool IDs();				//R13
		bool declaration();			//R12
		bool declarationList();			//R11
		bool optDeclarationList();		//R10
		bool body();				//R9
		bool qualifier();			//R8
		bool parameter();			//R7
		bool parameterList();			//R6
		bool optParameterList();		//R5
		bool function();			//R4
		bool functionDefinitions();		//R3
		bool optFunctionDefinitions();		//R2
		bool rat18F();				//R1
		// debug
		void error(int rule);
		void printTest();
		// asm
		void saveToken();
		void gen_instr(string, int);
		int get_address(Record);
		void push_jumpstack(int);
		int pop_jumpstack();
		void back_patch(int);
		void print_symbols();
		void print_instr();
};

//----------ASSEMBLY-----------------------------------------------------------------

void Grammar::saveToken() {
	// first, search symbol table to check whether id is already assigned
	bool isInTable = false;
	for (auto sym : sym_table) {
		if (cToken.lex == sym.token.lex) {
			// TODO
			// check whether duplicate symbol is same type
			//	if so, throw an error
			//if (cToken.tok == sym.token.tok) {
				isInTable = true;
				//cout << "ERROR: Symbol '" << cToken.lex << "' is already declared\n";
			//}
		}
	}

	// if cToken isn't in table, add it
	if (!isInTable) {
		save = cToken;
		symbol sym;		// temp symbol for insertion
		sym.token = cToken;
		sym.address = sym_address;
		sym_table.push_back(sym);
		sym_address++;
	}
}

void Grammar::gen_instr(string op, int oprnd) {
	instr_table[instr_address].address = instr_address;
	instr_table[instr_address].op = op;
	instr_table[instr_address].oprnd = oprnd;
 	instr_address++;
}

int Grammar::get_address(Record token) {
	for (auto sym : sym_table) {
		if (token.lex == sym.token.lex)
			return sym.address;
	}
	// token isn't found
	cout << "ERROR: '" << token.lex << "' has not been declared\n";
}

void Grammar::push_jumpstack(int instrAddress) {
	jumpstack.push_back(instrAddress);
}

int Grammar::pop_jumpstack() {
	int backElement = jumpstack.back();
	jumpstack.pop_back();
	return backElement;
}

void Grammar::back_patch(int jumpAddress) {
	temp_address = pop_jumpstack();
	instr_table[temp_address].oprnd = jumpAddress;
}

void Grammar::print_symbols() {
	asmFile << endl;
	asmFile << " Symbol Table" <<endl;
	asmFile << "--------------------------------------" << endl;
	asmFile << "Identifier    Memory Address    Type" << endl;
	for (auto sym : sym_table)
	{
		asmFile.width(14); asmFile << left << sym.token.lex;
		asmFile.width(18); asmFile << left << sym.address;
		asmFile.width(10); asmFile << left << sym.token.tok << endl;
	}
}

void Grammar::print_instr() {
	asmFile << "\n Instructions\n"
	     << "--------------------\n";
	for (int i = 1; i < instr_address; i++) {
		asmFile.width(5); asmFile << left << instr_table[i].address;
		asmFile.width(10); asmFile << left << instr_table[i].op;
		asmFile.width(10); asmFile << left;	
		instr_table[i].oprnd == NIL
			? asmFile << "" << endl
			: asmFile << instr_table[i].oprnd << endl;
	}
}

//----------DEFINITIONS------------------------------------------------------------
//----------GENERAL-----------------------------------------------------------------

Grammar::Grammar() {
	// base address; each element added to memory is assigned address+1
	instr_address = 1;
	sym_address = 5000;
}

void Grammar::switchT()
{
	tokenSwitch.tok = "";
	tokenSwitch.lex = "";
}

void Grammar::nextToken() {
	cToken = lexer();
	printToken();
}

void Grammar::printToken() {
	// TODO ERRORS MIGHT ARISE HERE
		outFile << setw(5) << left << "\nToken: " << cToken.tok << "         " << right << "Lexeme: " << cToken.lex << endl;
}
//Syntax Error: failed on non-terminal
//Syntax Error: current token failed on terminal
//----------GRAMMAR-----------------------------------------------------------------
//R1
bool Grammar::rat18F() {
	metafile.open("metafile.txt");
	nextToken();
	// set current token to empty
	switchT();
	bool isRat18F = true;
	//initializeTokenList()?
	outFile << "	<Rat18F> --> <Opt Function Definitions> $$ <Opt Declaration List> <Statement List> $$\n";
	if (optFunctionDefinitions()) {
		
		if (cToken.lex == "$$") {
			nextToken();
			if (optDeclarationList()) {
				if (statementList()) {
					
					if (cToken.lex == "$$") {
					}
					else {
						outFile << "		Syntax Error(At Rule 1): current token failed on terminal '$$'\n";
						isRat18F = false;
					}
				}
				else {
					outFile << "		Syntax Error(At Rule 1): failed on non-terminal <Statement List>\n";
					isRat18F = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 1): failed on non-terminal <Opt Declaration List\n";
				isRat18F = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 1): current token failed on terminal '$$'\n";
			isRat18F = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 1): failed on non-terminal <Opt Function Definitions>\n";
		isRat18F = false;
	}
	return isRat18F;
}
//R2
bool Grammar::optFunctionDefinitions() {
	bool isOptFunctionDefinitions = true;
	outFile<<"	<Opt Function Definitions> --> <Function Definitions> | <Empty>\n";
	if (functionDefinitions()) {
	}
	else {
	}
	return isOptFunctionDefinitions;
}
//R3
bool Grammar::functionDefinitions() {
	bool isFunctionDefinitions = true;
	outFile << "	<Function Definitions>  --> <Function> | <Function> <Function Definitions>\n";
	if (function()) {
		if (functionDefinitions()) {
			//repeat until its done with <Function>
		}
		else {
			//Means that <Function> is called once
		}
	}
	else if (cToken.lex == "$$")
	{
		isFunctionDefinitions = false;
	}
	else {
		outFile << "		Syntax Error(At Rule 3): failed on non-terminal <Function> \n";
		isFunctionDefinitions = false;
	}
	return isFunctionDefinitions;
}
//R4
bool Grammar::function() {
	bool isFunction = true;
	outFile << "	<Function> --> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n";
	if (cToken.lex == "function") {
		nextToken();
		if (cToken.tok == "identifier") {
			nextToken();
			if (cToken.lex == "(") {
				nextToken();
				if (optParameterList()) {
					if (cToken.lex == ")") {
						nextToken();
						if (optDeclarationList()) {
							if (body()) {
							}
							else {
								outFile << "		Syntax Error(At Rule 4): failed on non-terminal <Body>\n";
								isFunction = false;
							}
						}
						else {
							outFile << "		Syntax Error(At Rule 4): failed on non-terminal <Opt Declaration List>\n";
							isFunction = false;
						}
					}
					else {
						outFile << "		Syntax Error(At Rule 4): current token failed on terminal ')' \n";
						isFunction = false;
					}
				}
				else {
					outFile << "		Syntax Error(At Rule 4): failed on non-terminal <Opt Parameter List>\n";
					isFunction = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 4): current token failed on terminal ')' \n";
				isFunction = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 4): failed on non-terminal <Identifier>\n";
			isFunction = false;
		}
	}
	else if (cToken.lex == "$$")
	{
		isFunction = false;
	}
	else {
		outFile << "		Syntax Error(At Rule 4): current token failed on terminal 'function' \n";
		isFunction = false;
	}
	return isFunction;
}
//R5
bool Grammar::optParameterList() {
	bool isOptParameterList = true;
	outFile << "	<Opt Parameter List> --> <Parameter List> | <Empty>\n";
	if (parameterList()) {
	}
	else {
	}
	return isOptParameterList;
}
//R6 
bool Grammar::parameterList() {
	bool isParameterList = true;
	outFile << "	<ParameterList> --> <Parameter> | <Parameter> , <Parameter List>\n";
	if (parameter()) {
		if (cToken.lex == ",") {
			nextToken();
			if (parameterList()) {
				//true
			}
			else {
				outFile << "		Syntax Error(At Rule 6): failed on non-terminal <Paraneter List>\n";
				isParameterList = false;
			}
		}
		else {
			//first production 
		}		
	}
	else if (cToken.lex == ")")
	{
		//case if empty
		isParameterList = false;
	}
	else {
		outFile << "		Syntax Error(At Rule 6): failed on non-terminal <Paraneter>\n";
		isParameterList = false;	
	}
	return isParameterList;
}
//R7
bool Grammar::parameter() {
	bool isParameter = true;	
	outFile << "	<Parameter> --> <IDs> : <Qualifier>\n";
	if (IDs()) {
		if (cToken.lex == ":") {
			nextToken();
			if (qualifier()) {
			}
			else {
				outFile << "		Syntax Error(At Rule 7): failed on non-terminal <Qualifier>\n";
				isParameter = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 7): current token failed on terminal ':'\n";
			isParameter = false;
		}
	}
	else if (cToken.lex == ")")
	{
		//case if empty
		isParameter = false;
	}
	else {
		outFile << "		Syntax Error(At Rule 7): failed on non-terminal <IDs>\n";
		isParameter = false;
	}
	return isParameter;
}
// R8
bool Grammar::qualifier() {
	bool isQualifier = true;	
	outFile << "	<Qualifier> --> integer | boolean | real\n";
	if (!(cToken.lex == "int" || cToken.lex == "bool"
	   || cToken.lex == "real")) 
	{
		//dont print error for empty
		isQualifier = false;
	}
	else {
		nextToken();
	}
	return isQualifier;
}
//R9
bool Grammar::body() {
	bool isBody = true;
	outFile << "	<Body> --> { <Statement List> }\n";
	if (cToken.lex == "{") {
		nextToken();
		if (statementList()) {
			if (cToken.lex == "}") {
				nextToken();
			}
			else {
				outFile << "		Syntax Error(At Rule 9): current token failed on terminal '}'\n";
				isBody = false;
			}	
		}
		else {
			outFile << "		Syntax Error(At Rule 9): failed on non-terminal <Statement List>\n";
			isBody = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 9): current token failed on terminal '{'\n";
		isBody = false;
	}
	return isBody;
}
//R10
bool Grammar::optDeclarationList() {
	bool isOptDeclarationList = true;
	outFile << "	<Opt Declaration List> --> <Declaration List> | Empty\n";
	if (declarationList()) {
	}
	else {
	}
	return isOptDeclarationList;
}
// R11
bool Grammar::declarationList() {
	bool isDeclarationList = true;
	outFile << "	<Declaration List>  --> <Declaration> ;|<Declaration> ; <Declaration List>\n";
	if (declaration()) {
		if (cToken.lex == ";") {
			nextToken();
			if (declarationList()) {
			}
			else {
				//first right production true
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 11): current token failed on terminal ';'\n";
			isDeclarationList = false;
		}
	}
	else {
		//no error in case it is empty
		isDeclarationList = false;
	}
	return isDeclarationList;
}
// R12
bool Grammar::declaration() {
	bool isDeclaration = true;	
	outFile << "	<Declaration> --> <Qualifier> <IDs>\n";
	if (qualifier()) {
		if (IDs()) {
		}
		else {
			outFile << "		Syntax Error(At Rule 12): failed on non-terminal <IDs>\n";
		}
	}
	else {
		//don't print error in case of empty
		isDeclaration = false;
	}
	return isDeclaration;
}
// R13
bool Grammar::IDs() {
	bool isIDs = true;
	outFile << "	<IDs> --> <Identifier> | <Identifier> , <IDs> \n";
	if (cToken.tok == "identifier") {
		// asm
		saveToken();
		nextToken();
		if (cToken.lex == ",") {
			nextToken();
			if (IDs()) {
			}
			else {
				outFile << "		Syntax Error(At Rule 13): failed on non-terminal <IDs>\n";
				isIDs = false;
			}
		}
		else {
			//true first production
		}
	}
	else if (cToken.lex == ")")
	{
		//case if empty
		isIDs = false;
	}
	else {
		outFile << "		Syntax Error(At Rule 13): failed on non-terminal <Identifier>\n";
		isIDs = false;
	}
	return isIDs;
}
//R14
bool Grammar::statementList() {
	bool isStatementList = true;	
	outFile << "	<Statement List> = <Statement> | <Statement> <Statement List>\n";
	if (statement()) {
		if (statementList()) {
			//cout << "test1\n";
		}
		else {
			//cout << "test2\n";
			//first production true
		}
	}
	else {
		//cout << "test3\n";
		isStatementList = false;
	}	
	return isStatementList;
}
// R15
bool Grammar::statement() {
	bool isStatement = true;	
	outFile<< "	<Statement> -->  <Compound> | <Assign> | <If> |  <Return> | <Print> | <Scan> | <While>\n";
	if (cToken.lex == "{") {
		compound();
	}
	else if (cToken.tok == "identifier") {
		assign();
	}
	else if (cToken.lex == "if") {
		If();
	}
	else if (cToken.lex == "return") {
		return_();
	}
	else if (cToken.lex == "put") {
		print();
	}
	else if (cToken.lex == "get") {
		scan();
	}
	else if (cToken.lex == "while") {
		while_();
	}
	else {
		//cout << "test0\n";
		isStatement = false;
	}
	return isStatement;
}
//R16
bool Grammar::compound() {
	bool isCompound = true;
	outFile << "	<Compound> = { <Statement List> } \n";
	if (cToken.lex == "{") {
		nextToken();
		if (statementList()) {
			if (cToken.lex == "}") {
				nextToken();
			}
			else {
				outFile << "		Syntax Error(At Rule 16): current token failed on terminal '}'\n";
				isCompound = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 16): failed on non-terminal\n";
			isCompound = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 16): current token failed on terminal '{'\n";
		isCompound = false;
	}
	return isCompound;
}
// R17
bool Grammar::assign() {
	bool isAssign = true;	
	outFile << "	<Assign> --> <Identifier> = <Expression> ;\n";
	if (cToken.tok == "identifier") {
		// asm
		saveToken();
		nextToken();
		if (cToken.lex == "=") {
			nextToken();
			if (expression()) {
				if (cToken.lex == ";") {
					// asm
					gen_instr("POPM", get_address(save));
					nextToken();
				}
				else {
					outFile << "		Syntax Error(At Rule 17): current token failed on terminal ';'\n";
					isAssign = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 17): failed on non-terminal <Expression>\n";
				isAssign = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 17): current token failed on terminal '='\n";
			isAssign = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 17): failed on non-terminal <Identifier>\n";
		isAssign = false;
	}
	return isAssign;
}
// R18
bool Grammar::If(){
	bool isIf = true;
	outFile << "	<If> -> if (<Condition>) <Statement>   ifend |";
	outFile << "if (<Condition>) <Statement>   else  <Statement>  ifend\n";
	if (cToken.lex == "if")
	{
		temp_address = instr_address;
		nextToken();
		if (cToken.lex == "(")
		{
			nextToken();
			if (condition())
			{
				if (cToken.lex == ")")
				{
					nextToken();
					if (statement())
					{
						back_patch(instr_address);
						if(cToken.lex == "ifend")
						{ 
							nextToken();
						}
						else if (cToken.lex == "else")
						{
							nextToken();
							if (statement())
							{
								if (cToken.lex == "ifend")
									nextToken();
								else
								{
									outFile << "		Syntax Error(At Rule 18): current token failed on terminal 'ifend'\n";
									isIf = false;
								}
							}
							else
							{
								outFile << "		Syntax Error(At Rule 18): failed on non-terminal <statement> \n";
								isIf = false;
							}
						}
						else
						{
							outFile << "		Syntax Error(At Rule 18): current token failed on terminal 'ifend' or 'else'\n";
							isIf = false;
						}
					}
					else
					{
						outFile << "		Syntax Error(At Rule 18): failed on non-terminal <statement> \n";
						isIf = false;
					}
				}
				else
				{
					outFile << "		Syntax Error(At Rule 18): current token failed on terminal ')'\n";
					isIf = false;
				}
			}
			else
			{
				outFile << "		Syntax Error(At Rule 18): failed on non-terminal <condition>\n";
				isIf = false;
			}
		}
		else
		{
			outFile << "		Syntax Error(At Rule 18): current token failed on terminal '('\n";
			isIf = false;
		}
	}
	return isIf;
}
// R19
bool Grammar::return_() {
	bool isReturn = true;
	outFile << "	<Return> --> return ; | return <Expression> ;\n";
	if (cToken.lex == "return") {
		nextToken();
		if (cToken.lex == ";") {
			nextToken();
		}
		else if (expression()) {
			if (cToken.lex == ";") {
				nextToken();
			}
			else {
				outFile << "		Syntax Error(At Rule 19): current token failed on terminal ';'\n";
				isReturn = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 19): failed on current token failed terminal ';'";
			outFile <<"or  non-terminal <Expression>\n";
			isReturn = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 19): current token failed on terminal 'return'\n";
		isReturn = false;
	}
	return isReturn;
}
// R20
bool Grammar::print() {
	bool isPrint = true;
	outFile << "	<Print> -->  put ( <Expression> ) ;\n";
	if (cToken.lex == "put") {
		nextToken();
		if (cToken.lex == "(") {
			nextToken();
			if (expression()) {
				if (cToken.lex == ")") {
					nextToken();
					if (cToken.lex == ";") {
						// asm
						gen_instr("STDOUT", NIL);
						nextToken();
					}
					else {
						outFile << "		Syntax Error(At Rule 20): current token failed on terminal ';'\n";
						isPrint = false;
					}
				}
				else {
					outFile << "		Syntax Error(At Rule 20): current token failed on terminal ')'\n";
					isPrint = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 20): failed on non-terminal on <Expression>\n";
				isPrint = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 20): current token failed on terminal '('\n";
			isPrint = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 20): current token failed on terminal 'put'\n";
		isPrint = false;
	}
	return isPrint;
}
// R21
bool Grammar::scan() {
	bool isScan = true;
	outFile << "	<Scan> -->  get ( <IDs> ) ;\n";
	if (cToken.lex == "get") {
		// asm
		gen_instr("STDIN", NIL);
		nextToken();
		if (cToken.lex == "(") {
			nextToken();
			if (IDs()) {
				if (cToken.lex == ")") {
					nextToken();
					if (cToken.lex == ";") {
						nextToken();
					}
					else {
						outFile << "		Syntax Error(At Rule 21): current token failed on terminal ';'\n";
						isScan = false;
					}
				}
				else {
					outFile << "		Syntax Error(At Rule 21): current token failed on terminal ')'\n";
					isScan = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 21): failed on non-terminal <IDs>\n";
				isScan = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 21): current token failed on terminal '('\n";
			isScan = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 21): current token failed on terminal 'get'\n";
		isScan = false;
	}
	return isScan;
}

// R22
bool Grammar::while_() {
	bool isWhile = true;
	outFile << "	<While> --> while (<Condition>) <Statement> whileend\n";
	if (cToken.lex == "while") {
		// asm
		temp_address = instr_address;
		//push_jumpstack(instr_address); // prevents popping empty stack
		gen_instr("LABEL", NIL);
		nextToken();
		if (cToken.lex == "(") {
			nextToken();
			if (condition()) {
				if (cToken.lex == ")") {
					nextToken();
					if (statement()) {
						// asm
						gen_instr("JUMP", temp_address);
						back_patch(instr_address);
						if (cToken.lex == "whileend") {
							nextToken();
						}
						else {
							outFile << "		Syntax Error(At Rule 22): current token failed on terminal 'whileend'\n";
							isWhile = false;
						}
					}
					else {
						outFile << "		Syntax Error(At Rule 22): failed on non-terminal <Statement>\n";
						isWhile = false;
					}
				}
				else {
					outFile << "		Syntax Error(At Rule 22): current token failed on terminal ')'\n";
					isWhile = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 22): failed on non-terminal <Condition>\n";
				isWhile = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 22): current token failed on terminal '('\n";
			isWhile = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 22): current token failed on terminal 'while'\n";
		isWhile = false;
	}
	return isWhile;
}
// R23
bool Grammar::condition() {
	bool isCondition = true;
	outFile << "	<Condition> --> <Expression> <Relop> <Expression>\n";
	if (expression()) {
		if (relop()) {
			if (expression()) {
			}
			else {
				outFile << "		Syntax Error(At Rule 23): failed on non-terminal second <Expression>\n";
				isCondition = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 23): failed on non-terminal <Relop>\n";
			isCondition = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 23): failed on non-terminal <Expression>\n";
		isCondition = false;
	}
	return isCondition;
}

// R24
bool Grammar::relop() {
	bool isRelop = true;
	outFile << "	<<Relop> -->  == | ^= | > | < | => | =< \n";
	if (cToken.tok == "operator") {
		// asm
		if (cToken.lex == "==") {
			gen_instr("EQU", NIL);
			push_jumpstack(instr_address);
			gen_instr("JUMPZ", NIL);
		}
		else if (cToken.lex == "^=") {
			gen_instr("NEQ", NIL);
			push_jumpstack(instr_address);
			gen_instr("JUMPZ", NIL);
		}
		else if (cToken.lex == ">") {
			gen_instr("GRT", NIL);
			push_jumpstack(instr_address);
			gen_instr("JUMPZ", NIL);
		}
		else if (cToken.lex == "<") {
			gen_instr("LES", NIL);
			push_jumpstack(instr_address);
			gen_instr("JUMPZ", NIL);
		}
		else if (cToken.lex == "=>") {
			gen_instr("GEQ", NIL);
			push_jumpstack(instr_address);
			gen_instr("JUMPZ", NIL);
		}
		else if (cToken.lex == "=<") {
			gen_instr("LEQ", NIL);
			push_jumpstack(instr_address);
			gen_instr("JUMPZ", NIL);
		}
		nextToken();
	}
	else {
		outFile << "		Syntax Error(At Rule 24): failed on non-terminal <Relop>\n";
		isRelop = false;
	}
	return isRelop;
}
// R25
bool Grammar::expression() {
	bool isExpression = true;
	outFile << "	<Expression> --> <Term> <ExpressionPrime>\n";
	if (term()) {
		// nextToken();
		if (expressionPrime())
			isExpression = true;
		else {
			outFile << "		Syntax Error(At Rule 25): failed on non-terminal <ExpressionPrime>\n";
			isExpression = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 25): failed on non-terminal <Term>\n";
		isExpression = false;
	}
	return isExpression;
}
//R25 Prime
bool Grammar::expressionPrime() {
	bool isExpressionPrime = true;
	if (cToken.lex == "-" || cToken.lex == "+") {
		outFile << "	<ExpressionPrime> --> + <Term> <ExpressionPrime> | - <Term> <ExpressionPrime> |  Empty\n";
		nextToken();
		if (term()) {
			// asm
			gen_instr("ADD", NIL);
			// nextToken();
			if (expressionPrime())
				isExpressionPrime = true;
			else {
				outFile << "		Syntax Error(At Rule 25 prime): failed on non-terminal <ExpressionPrime>\n";
				isExpressionPrime = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 25 prime): failed on non-terminal <Term>\n";
			isExpressionPrime = false;
		}
	}
	else {
	}
	return isExpressionPrime;
}
// R26
bool Grammar::term() {
	bool isTerm = true;
	outFile << "	<Term> --> <Factor><TermPrime>\n";
	if (factor()) {//
				   // nextToken();
		if (termPrime())
			isTerm = true;
		else {
			outFile << "		Syntax Error(At Rule 26): failed on non-terminal on <TermPrime>\n";
			isTerm = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 26): failed on non-terminal <Factor>\n";
		isTerm = false;
	}
	return isTerm;
}
//R26 Prime
bool Grammar::termPrime() {
	bool isTermPrime = true;
	if (cToken.lex == "*" || cToken.lex == "/") {
		outFile << "	<TermPrime> --> * <Factor> <TermPrime> | / <Factor> <TermPrime> | Empty\n";
		nextToken();
		if (factor()) {
			gen_instr("MUL", NIL);
			if (termPrime())
				isTermPrime = true;
			else
			{
				outFile << "		Syntax Error(At Rule 26 prime): failed on non-terminal <termPrime>\n";
				isTermPrime = false;
			}
		}
		else {
			outFile << "		Syntax Error(At Rule 26 prime): failed on non-terminal <Factor>\n";
			isTermPrime = false;
		}
	}
	else {
	}
	return isTermPrime;
}
//R27
bool Grammar::factor() {
	bool isFactor = true;
	outFile << "	<Factor> --> - <Primary> | <Primary>\n";
	if (cToken.lex == "-") {
		if (primary()) {
		// asm
		// gen_instr("PUSHM", get_address(cToken));
		}
		else {
			outFile << "		Syntax Error(At Rule 27): failed on non-terminal <Primary>\n";
			isFactor = false;
		}
	}
	else if (primary()) {
		// asm
		// gen_instr("PUSHM", get_address(cToken));
	}
	else {
		outFile << "		Syntax Error(At Rule 27): failed on non-terminal <Primary> | <Primary>\n";
		isFactor = false;
	}
	return isFactor;
}
//R28
bool Grammar::primary() {
	bool isPrimary = true;
	outFile << "	<Primary> --> <Identifier> | <Integer> |";
	outFile <<"<Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false";
	if (cToken.tok == "integer" || cToken.tok == "real" ||
		cToken.lex == "true" || cToken.lex == "false") {
		// asm
		if (cToken.tok == "integer")
			gen_instr("PUSHI", stoi(cToken.lex));
		nextToken();
	}
	else if (cToken.tok == "identifier") {
		// asm
		gen_instr("PUSHM", get_address(cToken));
		nextToken();
		if (cToken.lex == "(") {
			nextToken();
			if (IDs()) {
				if (cToken.lex == ")") {
					nextToken();
				}
				else {
					outFile << "		Syntax Error(At Rule 28): current token failed on terminal ')'\n";
					isPrimary = false;
				}
			}
			else {
				outFile << "		Syntax Error(At Rule 28): failed on non-terminal <IDs>\n";
				isPrimary = false;
			}
		}
		else {
		}
	}
	else if (cToken.lex == "(") {
		nextToken();
		if (expression()) {
			if (cToken.lex == ")") {
				nextToken();
			}
			else {
				outFile << "		Syntax Error(At Rule 28): current token failed on terminal '}'\n";
				isPrimary = false;
			}
		}
		else {
			outFile << "		Syntax Error (At Rule 28): failed on non-terminal <Expression>\n";
			isPrimary = false;
		}
	}
	else {
		outFile << "		Syntax Error(At Rule 28): failed on <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false\n";
		isPrimary = false;
	}

	return isPrimary;
}
//R29
// Empty
//----------DEBUG-------------------------------------------------------------------
void Grammar::printTest() {
	if (rat18F()) 
	{
		outFile << "\n The test input is syntactically legal\n";	
	}

	cout << "Analysis complete. Syntax analysis and assembly instructions have been printed to \"syntax_analysis.txt\" and \"assembly.txt\" respectively.\n";
	
	// print assembly related stuff
	asmFile.open("assembly.txt");
	print_symbols();
	print_instr();
	asmFile.close();
}



#endif

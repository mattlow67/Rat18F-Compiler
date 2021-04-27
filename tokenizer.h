#ifndef TOKENIZER_H
#define TOKENIZER_H

#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<iomanip>
#include<cctype>
#include<vector>
using namespace std;
fstream metafile;
ofstream outFile;
//sets that contain the list for operators, keywords, and separators
set<string> op;
set<string> keywords;
set<string> sep;

// TODO replaced 'class' with 'struct'
//struct that allows lexer() to return a lexeme and token
struct Record {
	string lex;
	string tok;
};

// global list of tokens that is accessed by grammar functions
vector<Record> tokens;

// TODO created prototypes
void createlists();
Record lexer();
void createmeta();
void printTokens() {
	//remove comments from source code (rat file), create and store in metafile
	createmeta();
	metafile.open("metafile.txt");
	//fill the set variables with list of op, sep and keywords
	createlists();
	//object to store and display the lexeme and token
	Record rec;
	outFile.open("syntax_analysis.txt",ios::trunc | ios::out);
	outFile << "OUTPUT:" << endl << "token                   lexeme" << endl;
	outFile << "----------------------------------" << endl;
	//loop to get a token and a lexeme for the entire file 
	while (!metafile.eof())
		{
			//get a token and lexeme from lexer()
			rec = lexer();
			outFile.width(15);
			outFile << left << rec.tok << "         " << rec.lex << endl;
		}
	metafile.clear();
	metafile.seekg(0, ios::beg);
	metafile.close();
	// system("pause");
}

void createlists() {
	//create the list of sep, keywords, operators
	keywords.insert("function");
	keywords.insert("int");
	keywords.insert("boolean");
	keywords.insert("real");
	keywords.insert("if");
	keywords.insert("ifend");
	keywords.insert("else");
	keywords.insert("return");
	keywords.insert("put");
	keywords.insert("get");
	keywords.insert("while");
	keywords.insert("whileend");
	keywords.insert("true");
	keywords.insert("false");
	op.insert("=");
	op.insert("==");
	op.insert("^=");
	op.insert(">");
	op.insert("<");
	op.insert("=>");
	op.insert("=<");
	op.insert("+");
	op.insert("-");
	op.insert("*");
	op.insert("/");
	sep.insert(",");
	sep.insert(":");
	sep.insert("{");
	sep.insert("}");
	sep.insert(";");
	sep.insert("(");
	sep.insert(")");
	sep.insert("$$");
	sep.insert("$");
}

Record lexer() {
	Record rec;
	char lexC;
	bool done = false;
	int counter = 1;
	//starting state = 1
	int state = 1;
	//returning Record
	string lexS;
	
	if (!metafile.eof())
	{
	//get fisrt character to analyze
	metafile.get(lexC);
	lexS = lexC;
	//get a non space
	//quick solution to the lexical anaylzer find better way if possible
	while (lexS == " " || lexS == "\n" || lexS == "\t" || lexS == "\r")
	{
		if (counter > 100)
			break;
		metafile.get(lexC);
		lexS = lexC;
		counter++;
	}
		//create the table with qo=1 and accepting states
		//5(Int),6(Real),7(ID)
		int table[7][4] = { { 1,5,7,2 },
							{ 2,2,2,2 },
							{ 3,6,2,2 },
							{ 4,4,7,2 },
							{ 5,5,2,3 },
							{ 6,6,2,2 },
							{ 7,4,7,2 }, };
		//weird case after getting compound symbols at end of file
		if (lexC < 0)
		{
			done = true;
			rec.lex = " ";
		}
		//case if first character of lexeme is a space (ignored)
		else if (isspace(lexC))
		{
			done = true;
			rec.lex = lexS;
		}
		//case if operator
		else if (op.find(lexS) != op.end())
		{
			//check if compound / more than one operator character
			if ((lexC == '=' || lexC == '^') &&
				(metafile.peek() == '=' || metafile.peek() == '>' || metafile.peek() == '<'))
			{
				rec.lex = lexS;
				metafile.get(lexC);
				lexS = lexC;
				rec.lex = rec.lex + lexS;
			}
			else
				rec.lex = lexS;
			rec.tok = "operator";
			done = true;
		}
		//case if separators
		else if (sep.find(lexS) != sep.end())
		{
			//check if $$
			if (lexC == '$' && metafile.peek() == '$')
			{
				rec.lex = lexS;
				metafile.get(lexC);
				lexS = lexC;
				rec.lex = rec.lex + lexS;
			}
			else
				rec.lex = lexS;
			rec.tok = "separator";
			done = true;
		}
		//case if unknown character
		else if (!(isalpha(lexC) || isdigit(lexC) || lexC == '.'))
		{
			rec.tok = "unknown";
			rec.lex = lexS;
			done = true;
		}
		//prepare to use transitional table
		else
			rec.lex = lexS;
		//transitional table loop
		while (done == false && !metafile.eof())
		{

			//case if number
			if (isdigit(lexC))
			{
				state = table[state - 1][1];
			}
			//case if letter
			else if (isalpha(lexC))
			{
				state = table[state - 1][2];
			}
			//case if decimal(period)
			else if (lexC == '.')
			{
				state = table[state - 1][3];
			}
			//if any unknown, operator, or space loop end
			else
			{
				metafile.unget();
				rec.lex.pop_back();
				break;
			}
			//get a new character and add it to the lexeme string
			metafile.get(lexC);
			lexS = lexC;
			rec.lex = rec.lex + lexS;
		}
		//switch to react based on resulting state
		switch (state)
		{
			//no state 1 means its a space, op, or sep
		case 1:
			break;
			// state 2 results in token being unknown
		case 2:
		{
			rec.tok = "unknown";
			break;
		}
		// state 3 results in token being unknown
		case 3:
		{
			rec.tok = "unknown";
			break;
		}
		// state 4 results in token being unknown
		case 4:
		{
			rec.tok = "unknown";
			break;
		}
		// state 5 results in token being integer
		case 5:
		{
			rec.tok = "integer";
			break;
		}
		// state 6 results in token being real
		case 6:
		{
			rec.tok = "real";
			break;
		}
		// state 7 results in token being an identifier
		case 7:
		{
			//know check if the identifier is in keyword set
			if (keywords.find(rec.lex) != keywords.end())
				rec.tok = "keyword";
			else
				rec.tok = "identifier";
			break;
		}

		}
	}
	else
	{
		Record rec;
		rec.lex = "EOF";
		rec.tok = "EOF";
	}
	//return Record that contains string lexeme and string token
	return rec;
}

// TODO revised createmeta() so that strings are no longer required to omit comments
// createmeta() omits comments found in the original txt file and outputs a revised metafile
void createmeta() {
	// get name of file to be analyzed
	string fileName = "testinput1.txt";
	cout << "Enter file name for analysis:\ntestinput1.txt\ntestinput2.txt\ntestinput3.txt\n> ";
	getline(cin, fileName);

	if (!(fileName == "testinput1.txt" || fileName == "testinput2.txt" || fileName == "testinput3.txt"))
	{
		cout << "Invalid input.\nDefaulting to \"testinput1.txt\"\n\n";
		fileName = "testinput1.txt";
	}

	ifstream inFile;
	inFile.open(fileName.c_str());

	 if (!inFile.is_open()) {
		cout << "Cannot open file. Defaulting to testinput1.txt\n";
		fileName = "testinput1.txt";
		inFile.open(fileName.c_str());
	}
	
	// create/open metafile, and delete previous contents in "metafile.txt"
	metafile.open("metafile.txt", fstream::out | fstream::trunc);

	// read in inFile char by char, and if a comment is encountered, skip past it
	//	when writing to metafile
	char ch;
	while (inFile.get(ch)) {
		// if "[*" is encountered, begin comment loop
		if (ch == '[' && inFile.peek() == '*') {
			// skip "[" and go to "*", which will also be skipped by first
			//	loop iteration
			inFile.get(ch);
			while (inFile.get(ch) && !inFile.eof()) {
				if (ch == '*' && inFile.peek() == ']') {
						// skip past comment end bracket "*]"
						inFile.get(ch);
						inFile.get(ch);
						break;
					}
			}
		}
		// if (!inFile.eof()) needed?
		metafile << ch;
	}
	inFile.close();
	metafile.close();
}

#endif

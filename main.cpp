#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<iomanip>
#include<cctype>
#include<vector>
#include"tokenizer.h"
#include"grammar.h"
using namespace std;
int main() {
	printTokens();
	Grammar gra;
	gra.printTest();
	return 0;
}


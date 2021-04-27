## Rat18F Compiler
The compiling of code entails several processes, each of which produces intermediary code while adhering to the rules of the Rat18F language (which are provided in a separate file), and ultimately produces assembly code.

First, the source code is broken into smaller pieces, or tokens, which are validated according to Rat18F’s lexical conventions. A deterministic finite machine and its non-deterministic counterpart, as portrayed in the table and diagram below, are used for this task. 'd' denotes a digit and 'l' a letter/character.

> table and text go here

> diagram goes here

In the event when a lexeme is not identified as a separator, an operator, or an unknown token, it is run through a series of loops (as portrayed above and implemented in ```tokenizer.h```) to determine if it is an integer, real number, identifier, or keyword. 

Next, the syntax of program is analyzed for validity. The compiler, or more specifically, ```grammar.h```, iterates through the source code line-by-line while checking whether the syntax adheres to the grammar rules of the Rat18F language (the results of which can be viewed in ```syntax_analysis.txt```). The beginning of every syntactical analysis report derived from valid source code should look similar to the following:

```
Token: keyword         Lexeme: function
	<Rat18F> --> <Opt Function Definitions> $$ <Opt Declaration List> <Statement List> $$
	<Opt Function Definitions> --> <Function Definitions> | <Empty>
	<Function Definitions>  --> <Function> | <Function> <Function Definitions>
	<Function> --> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
```

Last, the "object code" produced from the previous step is converted to assembly in ```grammar.h``` with the use of a symbol table, which, in short, keeps track of declared and assigned variables and their respective scopes. By tracking each variable, the compiler prevents duplicate variables in the same scope. The finished assembly code is then printed to ```assembly.txt```.

## Instructions
* textinput1.txt, textinput2.txt, and txtinput3.txt are provided as source code

Windows:
	- Navigate to the folder containing ```asm.exe```
	- Run the executable ```asm.exe```
	- Enter the name of the txt file to be parsed (e.g., textinput1.txt)

Linux:
	- Navigate to the folder containing ```a.out```
	- Enter the following command
	  >./a.out
	- Enter the name of the txt file to be parsed (e.g., ```textinput1.txt```)

After running either program, open the file ```assembly.txt``` or ```syntax_analysis.txt``` to view the results.
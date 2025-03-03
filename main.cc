#include <iostream>
#include "parser.tab.hh"
#include "symboltable.h"

extern Node *root;
extern FILE *yyin;
extern int yylineno;
extern int lexical_errors;
extern yy::parser::symbol_type yylex();

enum errCodes
{
	SUCCESS = 0,
	LEXICAL_ERROR = 1,
	SYNTAX_ERROR = 2,
	AST_ERROR = 3,
	SEMANTIC_ERROR = 4,
	SEGMENTATION_FAULT = 139
};

int errCode = errCodes::SUCCESS;

// Handling Syntax Errors
void yy::parser::error(std::string const &err)
{
	if (!lexical_errors)
	{
		std::cerr << "Syntax errors found! See the logs below:" << std::endl;
		std::cerr << "\t@error at line " << yylineno << ". Cannot generate a syntax for this input:" << err.c_str() << std::endl;
		std::cerr << "End of syntax errors!" << std::endl;
		errCode = errCodes::SYNTAX_ERROR;
	}
}

int main(int argc, char **argv)
{
	// Flag to enable symbol table and semantic analysis
	bool doSemanticAnalysis = false;
	bool printSymbolTable = false;
	bool generateDotFile = false;

	// Parse command-line arguments
	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "-semantic")
		{
			doSemanticAnalysis = true;
		}
		else if (std::string(argv[i]) == "-printsymbols")
		{
			printSymbolTable = true;
		}
		else if (std::string(argv[i]) == "-st")
		{
			generateDotFile = true;
		}
	}

	// Reads from file if a file name is passed as an argument. Otherwise, reads from stdin.
	if (argc > 1 && argv[1][0] != '-')
	{
		if (!(yyin = fopen(argv[1], "r")))
		{
			perror(argv[1]);
			return 1;
		}
	}

	// Parse the input
	if (USE_LEX_ONLY)
	{
		yylex();
	}
	else
	{
		yy::parser parser;
		bool parseSuccess = !parser.parse();

		if (lexical_errors)
		{
			errCode = errCodes::LEXICAL_ERROR;
		}

		if (parseSuccess && !lexical_errors)
		{
			printf("\nThe compiler successfully generated a syntax tree for the given input! \n");

			try
			{
				// Print and generate AST
				printf("\nPrint Tree:  \n");
				root->print_tree();
				root->generate_tree();

				// Symbol table and semantic analysis phase
				if (doSemanticAnalysis || printSymbolTable || generateDotFile)
				{
					// Create the symbol table
					// Create the symbol table
					SymbolTable symbolTable;

					// Build the symbol table by traversing the AST
					buildSymbolTable(root, symbolTable);

					// Print the symbol table if requested
					if (printSymbolTable)
					{
						symbolTable.printSymbols();
					}

					// Generate DOT file for the symbol table if requested
					if (generateDotFile)
					{
						symbolTable.generateDotFile("symboltable.dot");
						system("dot -Tpdf symboltable.dot -osymboltable.pdf");
						std::cout << "Symbol table visualization saved to symboltable.pdf\n";
					}

					// Perform semantic analysis if requested
					if (doSemanticAnalysis)
					{
						std::cout << "\nPerforming Semantic Analysis...\n";
						bool semanticSuccess = performSemanticAnalysis(root, symbolTable);

						if (!semanticSuccess)
						{
							std::cerr << "Semantic analysis failed with errors.\n";
							errCode = errCodes::SEMANTIC_ERROR;
						}
						else
						{
							std::cout << "Semantic analysis completed successfully!\n";
						}
					}
				}
			}
			catch (...)
			{
				errCode = errCodes::AST_ERROR;
			}
		}
	}

	return errCode;
}
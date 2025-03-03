# Symbol Table Implementation for DV1655 Assignment 2

This document explains how the symbol table is implemented and how to use it for semantic analysis in the compiler.

## Overview

The symbol table implementation consists of the following main components:

1. **Symbol Classes**: Base class and derived classes for different identifier types:
   - `Symbol`: Base class for all symbols
   - `ClassSymbol`: For class declarations
   - `MethodSymbol`: For method declarations
   - `VariableSymbol`: For variable and parameter declarations

2. **SymbolTable Class**: Manages the collection of symbols with scope handling

3. **AST Traversal Functions**:
   - `buildSymbolTable()`: Populates the symbol table from AST
   - `performSemanticAnalysis()`: Uses the symbol table to check for semantic errors

## How to Use the Symbol Table

### Basic Usage

1. Create a symbol table:
```cpp
SymbolTable symbolTable;
```

2. Build the symbol table by traversing your AST:
```cpp
buildSymbolTable(root, symbolTable);
```

3. Perform semantic analysis using the populated symbol table:
```cpp
bool semanticSuccess = performSemanticAnalysis(root, symbolTable);
```

4. Print the contents of the symbol table for debugging:
```cpp
symbolTable.printSymbols();
```

5. Generate a visualization of the symbol table:
```cpp
symbolTable.generateDotFile("symboltable.dot");
// To convert to PDF (requires graphviz)
system("dot -Tpdf symboltable.dot -osymboltable.pdf");
```

### Command-Line Options

The compiler now supports the following command-line flags for symbol table and semantic analysis:

* `-semantic`: Enable semantic analysis on the input file
* `-printsymbols`: Print the contents of the symbol table 
* `-st`: Generate a visualization of the symbol table in PDF format

Example:
```bash
./compiler test_files/valid/Factorial.java -printsymbols
./compiler test_files/semantic_errors/DuplicateIdentifiers.java -semantic
```

### Symbol Table API

The `SymbolTable` class provides the following key methods:

* **Symbol Management**:
  * `addSymbol(Symbol* symbol)`: Add a symbol to the table
  * `getSymbol(const std::string& symbolName)`: Look up a symbol by name
  * `isSymbolInTable(const std::string& symbolName)`: Check if a symbol exists
  * `isDuplicateIdentifier(const std::string& name, int scope)`: Check for duplicate identifiers in the same scope

* **Scope Management**:
  * `enterScope()`: Enter a new scope level (increment scope counter)
  * `exitScope()`: Exit the current scope (decrement scope counter)
  * `getCurrentScope()`: Get the current scope level
  * `getSymbolsByScope(int scope)`: Get all symbols in a particular scope

* **Semantic Analysis Helpers**:
  * `checkTypes(const std::string& type1, const std::string& type2)`: Check if two types are compatible
  * `isUndeclaredIdentifier(const std::string& name)`: Check if an identifier is not declared

## Implementation Details

### Symbol Table Construction

The symbol table is constructed in a single left-to-right traversal of the AST using the `buildSymbolTable()` function. This function:

1. Processes declarations of classes, methods, and variables
2. Records scope information for each identifier
3. Handles nested scopes (class-level and method-level)

### Semantic Analysis Process

The semantic analysis is performed in a separate pass over the AST using the `performSemanticAnalysis()` function. This function:

1. Verifies that all identifiers used are properly declared
2. Checks for duplicate declarations in the same scope
3. Performs type checking for expressions, statements, and method calls
4. Reports all semantic errors found (doesn't stop at the first error)

## Extending the Implementation

To extend the semantic analysis functionality:

1. Add additional checks in the `performSemanticAnalysis()` function
2. Add helper methods to the `SymbolTable` class as needed
3. Enhance the `checkTypes()` method to handle more complex type compatibility checks

## Debugging Tips

1. Use `symbolTable.printSymbols()` to see the current state of the symbol table
2. Generate visualization with `symbolTable.generateDotFile()` to see the scope hierarchy
3. Test with both valid and invalid Java programs to ensure correct behavior
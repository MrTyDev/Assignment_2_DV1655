#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Node.h"

// Forward declarations
class Symbol;
class ClassSymbol;
class MethodSymbol;
class VariableSymbol;
class SymbolTable;

// Base Symbol class
class Symbol {
public:
    std::string name;
    std::string type;
    int scope;
    
    Symbol(std::string name, std::string type, int scope);
    virtual ~Symbol() = default;
    virtual void printSymbol() const;
    virtual std::string getKind() const { return "Symbol"; }
};

// Class symbol
class ClassSymbol : public Symbol {
public:
    std::string parentClass; // For inheritance
    
    ClassSymbol(std::string name, int scope, std::string parentClass = "");
    std::string getKind() const override { return "Class"; }
};

// Method symbol
class MethodSymbol : public Symbol {
public:
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters; // (name, type) pairs
    std::string classOwner;
    
    MethodSymbol(std::string name, std::string returnType, int scope, std::string classOwner);
    void addParameter(std::string name, std::string type);
    std::string getKind() const override { return "Method"; }
};

// Variable symbol
class VariableSymbol : public Symbol {
public:
    bool isArray;
    std::string ownerScope; // Class name or method name
    
    VariableSymbol(std::string name, std::string type, int scope, bool isArray, std::string ownerScope);
    std::string getKind() const override { return "Variable"; }
};

// Symbol Table class
class SymbolTable {
private:
    std::vector<Symbol*> table;
    int currentScope;
    
public:
    SymbolTable();
    ~SymbolTable();
    
    void addSymbol(Symbol* symbol);
    bool isSymbolInTable(const std::string& symbolName) const;
    bool isSymbolInScope(const std::string& symbolName, int scope) const;
    Symbol* getSymbol(const std::string& symbolName);
    std::vector<Symbol*> getSymbolsByScope(int scope) const;
    
    void enterScope();
    void exitScope();
    int getCurrentScope() const;
    
    void printSymbols() const;
    void generateDotFile(const std::string& filename) const;
    
    // Semantic analysis helpers
    bool checkTypes(const std::string& type1, const std::string& type2) const;
    bool isUndeclaredIdentifier(const std::string& name) const;
    bool isDuplicateIdentifier(const std::string& name, int scope) const;
};

// AST traversal function to build symbol table
void buildSymbolTable(Node* node, SymbolTable& symbolTable, std::string currentClass = "", std::string currentMethod = "");

// Semantic analysis function
bool performSemanticAnalysis(Node* node, SymbolTable& symbolTable);

#endif // SYMBOLTABLE_H
#include "symboltable.h"
#include <iostream>
#include <fstream>
#include <string>

// Symbol implementation
Symbol::Symbol(std::string name, std::string type, int scope)
    : name(std::move(name)), type(std::move(type)), scope(scope) {}

void Symbol::printSymbol() const {
    std::cout << getKind() << " | Name: " << name << ", Type: " << type << ", Scope: " << scope << std::endl;
}

// ClassSymbol implementation
ClassSymbol::ClassSymbol(std::string name, int scope, std::string parentClass)
    : Symbol(std::move(name), "class", scope), parentClass(std::move(parentClass)) {}

// MethodSymbol implementation
MethodSymbol::MethodSymbol(std::string name, std::string returnType, int scope, std::string classOwner)
    : Symbol(std::move(name), "method", scope), returnType(std::move(returnType)), classOwner(std::move(classOwner)) {}

void MethodSymbol::addParameter(std::string name, std::string type) {
    parameters.emplace_back(std::move(name), std::move(type));
}

// VariableSymbol implementation
VariableSymbol::VariableSymbol(std::string name, std::string type, int scope, bool isArray, std::string ownerScope)
    : Symbol(std::move(name), std::move(type), scope), isArray(isArray), ownerScope(std::move(ownerScope)) {}

// SymbolTable implementation
SymbolTable::SymbolTable() : currentScope(0) {}

SymbolTable::~SymbolTable() {
    for (auto& symbol : table) {
        delete symbol;
    }
}

void SymbolTable::addSymbol(Symbol* symbol) {
    if (isDuplicateIdentifier(symbol->name, symbol->scope)) {
        std::cerr << "Semantic Error: Duplicate identifier '" << symbol->name 
                 << "' in scope " << symbol->scope << std::endl;
    } else {
        table.push_back(symbol);
    }
}

bool SymbolTable::isSymbolInTable(const std::string& symbolName) const {
    for (const auto& symbol : table) {
        if (symbol->name == symbolName) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::isSymbolInScope(const std::string& symbolName, int scope) const {
    for (const auto& symbol : table) {
        if (symbol->name == symbolName && symbol->scope == scope) {
            return true;
        }
    }
    return false;
}

Symbol* SymbolTable::getSymbol(const std::string& symbolName) {
    for (auto& symbol : table) {
        if (symbol->name == symbolName) {
            return symbol;
        }
    }
    return nullptr;
}

std::vector<Symbol*> SymbolTable::getSymbolsByScope(int scope) const {
    std::vector<Symbol*> result;
    for (auto& symbol : table) {
        if (symbol->scope == scope) {
            result.push_back(symbol);
        }
    }
    return result;
}

void SymbolTable::enterScope() {
    currentScope++;
}

void SymbolTable::exitScope() {
    if (currentScope > 0) {
        currentScope--;
    }
}

int SymbolTable::getCurrentScope() const {
    return currentScope;
}

void SymbolTable::printSymbols() const {
    std::cout << "\n===== SYMBOL TABLE =====\n";
    if (table.empty()) {
        std::cout << "Symbol table is empty!" << std::endl;
        return;
    }

    for (const auto& symbol : table) {
        symbol->printSymbol();
    }
    std::cout << "=======================\n\n";
}

void SymbolTable::generateDotFile(const std::string& filename) const {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    dotFile << "digraph SymbolTable {\n";
    dotFile << "  node [shape=record];\n";

    // Group symbols by scope
    std::unordered_map<int, std::vector<Symbol*>> scopeMap;
    for (const auto& symbol : table) {
        scopeMap[symbol->scope].push_back(symbol);
    }

    // Create nodes for each scope
    for (const auto& pair : scopeMap) {
        dotFile << "  subgraph cluster_" << pair.first << " {\n";
        dotFile << "    label=\"Scope " << pair.first << "\";\n";
        
        for (const auto& symbol : pair.second) {
            std::string nodeId = symbol->getKind() + "_" + symbol->name + "_" + std::to_string(symbol->scope);
            dotFile << "    " << nodeId << " [label=\"{" << symbol->getKind() << "|Name: " << symbol->name;
            dotFile << "|Type: " << symbol->type << "}\"];\n";
        }
        
        dotFile << "  }\n";
    }
    
    dotFile << "}\n";
    dotFile.close();
    std::cout << "Generated DOT file: " << filename << std::endl;
}

bool SymbolTable::checkTypes(const std::string& type1, const std::string& type2) const {
    // Basic type checking - equality check
    if (type1 == type2) return true;
    
    // Handle array types
    if (type1 == "int[]" && type2 == "int[]") return true;
    
    // TODO: Add more sophisticated type checking for class inheritance
    
    return false;
}

bool SymbolTable::isUndeclaredIdentifier(const std::string& name) const {
    return !isSymbolInTable(name);
}

bool SymbolTable::isDuplicateIdentifier(const std::string& name, int scope) const {
    return isSymbolInScope(name, scope);
}

// Helper function to get the first child of a node if it exists
Node* getFirstChild(Node* node) {
    if (!node || node->children.empty()) return nullptr;
    return node->children.front();
}

// AST traversal to build symbol table
void buildSymbolTable(Node* node, SymbolTable& symbolTable, std::string currentClass, std::string currentMethod) {
    if (!node) return;
    
    // Process current node
    if (node->type == "ClassDeclaration") {
        // When entering a class declaration
        currentClass = node->value;
        
        // Check if this is an extending class
        std::string parentClass = "";
        for (auto child : node->children) {
            if (child->type == "Extends") {
                parentClass = child->value;
                break;
            }
        }
        
        // Add class to symbol table
        symbolTable.addSymbol(new ClassSymbol(node->value, symbolTable.getCurrentScope(), parentClass));
        
        // Process class members with new scope
        symbolTable.enterScope();
        for (auto child : node->children) {
            buildSymbolTable(child, symbolTable, currentClass, "");
        }
        symbolTable.exitScope();
        return;
    } 
    else if (node->type == "MethodDeclaration") {
        // When entering a method declaration
        currentMethod = node->value;
        std::string returnType = "";
        
        // First child is the return type
        if (!node->children.empty()) {
            Node* typeNode = node->children.front();
            returnType = typeNode->value;
        }
        
        // Add method to symbol table
        MethodSymbol* methodSymbol = new MethodSymbol(node->value, returnType, symbolTable.getCurrentScope(), currentClass);
        
        // Process parameters
        for (auto child : node->children) {
            if (child->type == "ParameterList") {
                for (auto param : child->children) {
                    if (param->type == "Parameter") {
                        std::string paramType = "";
                        if (!param->children.empty()) {
                            paramType = param->children.front()->value;
                        }
                        methodSymbol->addParameter(param->value, paramType);
                    }
                }
            }
        }
        
        symbolTable.addSymbol(methodSymbol);
        
        // Process method body with new scope
        symbolTable.enterScope();
        for (auto child : node->children) {
            buildSymbolTable(child, symbolTable, currentClass, currentMethod);
        }
        symbolTable.exitScope();
        return;
    }
    else if (node->type == "VarDeclaration" || node->type == "ArrayDeclaration") {
        // Process variable declarations
        std::string varType = "";
        bool isArray = (node->type == "ArrayDeclaration");
        
        // Get variable type
        if (!node->children.empty()) {
            varType = node->children.front()->value;
        }
        
        // Add variable to symbol table
        std::string ownerScope = currentMethod.empty() ? currentClass : currentMethod;
        symbolTable.addSymbol(new VariableSymbol(node->value, varType, symbolTable.getCurrentScope(), isArray, ownerScope));
    }
    else if (node->type == "Parameter") {
        // Process parameters as variables
        std::string paramType = "";
        
        // Get parameter type
        if (!node->children.empty()) {
            paramType = node->children.front()->value;
        }
        
        // Add parameter to symbol table
        symbolTable.addSymbol(new VariableSymbol(node->value, paramType, symbolTable.getCurrentScope(), false, currentMethod));
    }
    
    // Continue traversal for all children
    for (auto child : node->children) {
        buildSymbolTable(child, symbolTable, currentClass, currentMethod);
    }
}

// Helper function to get the type of an expression node
std::string getExpressionType(Node* node, SymbolTable& symbolTable) {
    if (!node) return "error";
    
    if (node->type == "Int") {
        return "int";
    } 
    else if (node->type == "Boolean") {
        return "boolean";
    } 
    else if (node->type == "Identifier") {
        // Look up identifier type in symbol table
        Symbol* symbol = symbolTable.getSymbol(node->value);
        if (symbol) {
            return symbol->type;
        } else {
            return "error"; // Undeclared identifier
        }
    } 
    else if (node->type == "This") {
        // 'this' refers to the current class
        // We need to know the current class context to determine its type
        // For now, return a placeholder
        return "this";
    }
    else if (node->type == "NewObject") {
        // New object type is the class name
        return node->value;
    }
    else if (node->type == "NewArray") {
        return "int[]";
    }
    else if (node->type == "ArrayAccess") {
        // Array access returns the element type
        if (node->children.empty()) return "error";
        std::string arrayType = getExpressionType(node->children.front(), symbolTable);
        if (arrayType == "int[]") {
            return "int";
        }
        return "error"; // Not an array type
    }
    else if (node->type == "Length") {
        // Length operation returns an integer
        return "int";
    }
    else if (node->type == "MethodCall") {
        // Need to look up method return type
        // This is simplified - would need to resolve the class and method
        Symbol* methodSymbol = symbolTable.getSymbol(node->value);
        if (methodSymbol && methodSymbol->getKind() == "Method") {
            MethodSymbol* method = static_cast<MethodSymbol*>(methodSymbol);
            return method->returnType;
        }
        return "error";
    }
    else if (node->type == "AddExpression" || node->type == "SubExpression" || 
             node->type == "MultExpression") {
        // Arithmetic operations should be on integers and return integer
        if (node->children.size() < 2) return "error";
        
        auto it = node->children.begin();
        std::string leftType = getExpressionType(*it, symbolTable);
        ++it;
        std::string rightType = getExpressionType(*it, symbolTable);
        
        if (leftType == "int" && rightType == "int") {
            return "int";
        }
        return "error"; // Type mismatch
    }
    else if (node->type == "AndExpression" || node->type == "OrExpression") {
        // Logical operations should be on booleans and return boolean
        if (node->children.size() < 2) return "error";
        
        auto it = node->children.begin();
        std::string leftType = getExpressionType(*it, symbolTable);
        ++it;
        std::string rightType = getExpressionType(*it, symbolTable);
        
        if (leftType == "boolean" && rightType == "boolean") {
            return "boolean";
        }
        return "error"; // Type mismatch
    }
    else if (node->type == "LessThanExpression" || node->type == "EqualExpression") {
        // Comparison operations should be on integers and return boolean
        if (node->children.size() < 2) return "error";
        
        auto it = node->children.begin();
        std::string leftType = getExpressionType(*it, symbolTable);
        ++it;
        std::string rightType = getExpressionType(*it, symbolTable);
        
        if (leftType == rightType && (leftType == "int" || leftType == "boolean")) {
            return "boolean";
        }
        return "error"; // Type mismatch
    }
    else if (node->type == "NotExpression") {
        // Not operation should be on boolean and return boolean
        if (node->children.empty()) return "error";
        
        std::string exprType = getExpressionType(node->children.front(), symbolTable);
        if (exprType == "boolean") {
            return "boolean";
        }
        return "error"; // Type mismatch
    }
    
    // Default case - couldn't determine the type
    return "unknown";
}

// Enhanced semantic analysis implementation
bool performSemanticAnalysis(Node* node, SymbolTable& symbolTable) {
    if (!node) return true;
    
    bool result = true;
    
    // Check for semantic errors based on node type
    if (node->type == "Identifier") {
        // Check if identifier is declared
        if (symbolTable.isUndeclaredIdentifier(node->value)) {
            std::cerr << "Semantic Error at line " << node->lineno 
                     << ": Undeclared identifier '" << node->value << "'" << std::endl;
            result = false;
        }
    } 
    else if (node->type == "AssignStatement") {
        // Check variable assignment
        // 1. Check if variable is declared
        if (symbolTable.isUndeclaredIdentifier(node->value)) {
            std::cerr << "Semantic Error at line " << node->lineno 
                     << ": Assignment to undeclared variable '" << node->value << "'" << std::endl;
            result = false;
        }
        // 2. Type check: left-hand side and right-hand side must have compatible types
        else if (!node->children.empty()) {
            Symbol* varSymbol = symbolTable.getSymbol(node->value);
            std::string lhsType = varSymbol ? varSymbol->type : "error";
            std::string rhsType = getExpressionType(node->children.front(), symbolTable);
            
            if (!symbolTable.checkTypes(lhsType, rhsType)) {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Type mismatch in assignment to '" << node->value 
                         << "'. Expected '" << lhsType << "', got '" << rhsType << "'" << std::endl;
                result = false;
            }
        }
    }
    else if (node->type == "ArrayAssignStatement") {
        // Check array assignment
        // 1. Check if array variable is declared
        if (symbolTable.isUndeclaredIdentifier(node->value)) {
            std::cerr << "Semantic Error at line " << node->lineno 
                     << ": Assignment to undeclared array '" << node->value << "'" << std::endl;
            result = false;
        }
        else {
            // 2. Check if it's actually an array type
            Symbol* arraySymbol = symbolTable.getSymbol(node->value);
            if (arraySymbol && arraySymbol->type != "int[]") {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Type '" << arraySymbol->type << "' is not an array type" << std::endl;
                result = false;
            }
            
            // 3. Check if index is an integer
            if (!node->children.empty()) {
                auto it = node->children.begin();
                std::string indexType = getExpressionType(*it, symbolTable);
                if (indexType != "int") {
                    std::cerr << "Semantic Error at line " << node->lineno 
                             << ": Array index must be an integer, got '" << indexType << "'" << std::endl;
                    result = false;
                }
                
                // 4. Check if the value assigned is compatible with the array element type
                ++it;
                if (it != node->children.end()) {
                    std::string valueType = getExpressionType(*it, symbolTable);
                    if (valueType != "int") {
                        std::cerr << "Semantic Error at line " << node->lineno 
                                 << ": Cannot assign '" << valueType << "' to element of int array" << std::endl;
                        result = false;
                    }
                }
            }
        }
    }
    else if (node->type == "IfStatement" || node->type == "WhileStatement") {
        // Check if condition is a boolean expression
        if (!node->children.empty()) {
            std::string conditionType = getExpressionType(node->children.front(), symbolTable);
            if (conditionType != "boolean") {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Condition must be of type boolean, got '" << conditionType << "'" << std::endl;
                result = false;
            }
        }
    }
    else if (node->type == "PrintStatement") {
        // Check if print statement argument is an integer
        if (!node->children.empty()) {
            std::string exprType = getExpressionType(node->children.front(), symbolTable);
            if (exprType != "int") {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Print statement requires integer expression, got '" << exprType << "'" << std::endl;
                result = false;
            }
        }
    }
    else if (node->type == "MethodCall") {
        // Check method call
        // 1. Check if method exists
        Symbol* symbol = symbolTable.getSymbol(node->value);
        if (!symbol || symbol->getKind() != "Method") {
            std::cerr << "Semantic Error at line " << node->lineno 
                     << ": Undefined method '" << node->value << "'" << std::endl;
            result = false;
        }
        else {
            // 2. Check parameter count and types
            MethodSymbol* method = static_cast<MethodSymbol*>(symbol);
            
            // Find the argument list node
            Node* argListNode = nullptr;
            if (node->children.size() > 1) {
                auto it = node->children.begin();
                ++it; // Skip the caller object
                argListNode = *it;
            }
            
            if (argListNode && argListNode->type == "ExpressionList") {
                // Count the arguments
                int argCount = argListNode->children.size();
                int paramCount = method->parameters.size();
                
                if (argCount != paramCount) {
                    std::cerr << "Semantic Error at line " << node->lineno 
                             << ": Method '" << node->value << "' expects " << paramCount 
                             << " parameters but got " << argCount << std::endl;
                    result = false;
                }
                else {
                    // Check each argument type against parameter type
                    auto argIt = argListNode->children.begin();
                    auto paramIt = method->parameters.begin();
                    
                    for (int i = 0; i < argCount; ++i, ++argIt, ++paramIt) {
                        std::string argType = getExpressionType(*argIt, symbolTable);
                        std::string paramType = paramIt->second; // second is the type
                        
                        if (!symbolTable.checkTypes(paramType, argType)) {
                            std::cerr << "Semantic Error at line " << node->lineno 
                                     << ": Parameter type mismatch in call to '" << node->value 
                                     << "'. Parameter " << (i+1) << " expects '" << paramType 
                                     << "', got '" << argType << "'" << std::endl;
                            result = false;
                        }
                    }
                }
            }
        }
    }
    else if (node->type == "Return") {
        // Check if return type matches method return type
        // This requires knowing which method we're in - would need method context
        // For simplicity, this check is limited
        if (!node->children.empty()) {
            std::string returnType = getExpressionType(node->children.front(), symbolTable);
            // We would need to check against the method's declared return type
            // This would require passing method context through the traversal
        }
    }
    else if (node->type == "ArrayAccess") {
        // Check if array index is an integer
        if (node->children.size() >= 2) {
            auto it = node->children.begin();
            std::string arrayType = getExpressionType(*it, symbolTable);
            if (arrayType != "int[]") {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Array access requires array type, got '" << arrayType << "'" << std::endl;
                result = false;
            }
            
            ++it;
            std::string indexType = getExpressionType(*it, symbolTable);
            if (indexType != "int") {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Array index must be an integer, got '" << indexType << "'" << std::endl;
                result = false;
            }
        }
    }
    else if (node->type == "Length") {
        // Check if length operation is applied to an array
        if (!node->children.empty()) {
            std::string exprType = getExpressionType(node->children.front(), symbolTable);
            if (exprType != "int[]") {
                std::cerr << "Semantic Error at line " << node->lineno 
                         << ": Length operator requires array type, got '" << exprType << "'" << std::endl;
                result = false;
            }
        }
    }
    
    // Recursively check all children
    for (auto child : node->children) {
        result = performSemanticAnalysis(child, symbolTable) && result;
    }
    
    return result;
}

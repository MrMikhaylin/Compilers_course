#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

// Типы символов (переменная, метод, класс)
enum class SymbolKind {
    VARIABLE,
    METHOD,
    CLASS
};

class Symbol {
public:
    std::string name;
    SymbolKind kind;
    
    Symbol(const std::string& n, SymbolKind k) : name(n), kind(k) {}
    virtual ~Symbol() = default;
};

class VariableSymbol : public Symbol {
public:
    std::string type;  // "int" пока только int
    
    VariableSymbol(const std::string& n, const std::string& t) 
        : Symbol(n, SymbolKind::VARIABLE), type(t) {}
};

class MethodSymbol : public Symbol {
public:
    std::string returnType;
    std::vector<std::string> parameters;
    
    MethodSymbol(const std::string& n, const std::string& ret) 
        : Symbol(n, SymbolKind::METHOD), returnType(ret) {}
};

class ClassSymbol : public Symbol {
public:
    std::unordered_map<std::string, std::unique_ptr<VariableSymbol>> fields;
    std::unordered_map<std::string, std::unique_ptr<MethodSymbol>> methods;
    
    ClassSymbol(const std::string& n) : Symbol(n, SymbolKind::CLASS) {}
};

class Scope {
private:
    std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols;
    Scope* parent;  // сырой указатель, т.к. родитель владеет детьми
    std::vector<std::unique_ptr<Scope>> children;
    std::string name;  // для отладки
    
public:
    Scope(Scope* parentScope = nullptr, const std::string& scopeName = "anonymous");
    
    bool addSymbol(std::unique_ptr<Symbol> symbol);
    
    Symbol* lookup(const std::string& name);
    
    Symbol* lookupLocal(const std::string& name);
    
    Scope* createChild(const std::string& scopeName = "anonymous");
    
    Scope* getParent() const { return parent; }
    const std::vector<std::unique_ptr<Scope>>& getChildren() const { return children; }
    const std::unordered_map<std::string, std::unique_ptr<Symbol>>& getSymbols() const { return symbols; }
    
    void dump(int indent = 0);
};
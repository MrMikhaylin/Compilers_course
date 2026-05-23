#include "scope.hpp"
#include <iostream>

Scope::Scope(Scope* parentScope, const std::string& scopeName) 
    : parent(parentScope), name(scopeName) {}

bool Scope::addSymbol(std::unique_ptr<Symbol> symbol) {
    const std::string& symName = symbol->name;
    
    if (symbols.find(symName) != symbols.end()) {
        return false;
    }
    
    symbols[symName] = std::move(symbol);
    return true;
}

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    
    if (parent) {
        return parent->lookup(name);
    }
    
    return nullptr;
}

Symbol* Scope::lookupLocal(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    return nullptr;
}

Scope* Scope::createChild(const std::string& scopeName) {
    auto child = std::make_unique<Scope>(this, scopeName);
    Scope* rawPtr = child.get();
    children.push_back(std::move(child));
    return rawPtr;
}

void Scope::dump(int indent) {
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "Scope: " << name << std::endl;
    
    for (const auto& [symName, sym] : symbols) {
        for (int i = 0; i < indent + 1; i++) std::cout << "  ";
        std::cout << "  Symbol: " << symName;
        if (auto var = dynamic_cast<VariableSymbol*>(sym.get())) {
            std::cout << " (variable, type: " << var->type << ")";
        }
        std::cout << std::endl;
    }
    
    for (const auto& child : children) {
        child->dump(indent + 1);
    }
}
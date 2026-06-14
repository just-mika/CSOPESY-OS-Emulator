#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>

enum class PrimitiveType
{
    INT,
    CHAR,
    FLOAT
};

typedef std::variant<int, char, float> PrimitiveValue;

struct Symbol
{
    PrimitiveType type;
    PrimitiveValue value;
};

class SymbolTable
{
public:
    void setVariable(const std::string& name, PrimitiveType type, PrimitiveValue value);

    PrimitiveValue getVariable(const std::string& name);

    bool hasVariable(const std::string& name) const;

private:
    std::unordered_map<std::string, Symbol> table;
};
#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <cstdint> // Needed for uint16_t

enum class PrimitiveType
{
    UINT16,
    CHAR,
    FLOAT
};

typedef std::variant<uint16_t, char, float> PrimitiveValue;

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
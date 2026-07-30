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

typedef std::variant<std::monostate, uint16_t, char, float> PrimitiveValue;

struct Symbol
{
    PrimitiveType type;
    PrimitiveValue value;
};

class SymbolTable
{
public:
    static constexpr size_t MAX_VARIABLES = 32; // Fixed 64-byte segment size (32 * 2 bytes)

    void setVariable(const std::string& name, PrimitiveType type, PrimitiveValue value);

    Symbol getVariable(const std::string& name);

    bool hasVariable(const std::string& name) const;

	size_t getVariableCount() const; // Get the current number of variables in the symbol table

private:
    std::unordered_map<std::string, Symbol> table;
};
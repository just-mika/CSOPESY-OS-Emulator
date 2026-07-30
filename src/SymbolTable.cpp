#include "SymbolTable.h"
#include <unordered_map> 
#include <algorithm>

void SymbolTable::setVariable(const std::string& name, PrimitiveType type, PrimitiveValue value)
{
    // If table limit reached (32 variables max) and it's a new variable declaration, ignore it
    if (!hasVariable(name) && table.size() >= MAX_VARIABLES)
    {
        return;
    }

    // Clamp UINT16 values between 0 and 65535
    if (type == PrimitiveType::UINT16)
    {
        if (std::holds_alternative<uint16_t>(value))
        {
            uint16_t val = std::get<uint16_t>(value);
            value = static_cast<uint16_t>(std::clamp<uint32_t>(val, 0, 65535));
        }
    }

    Symbol s = { type, value };
    this->table[name] = s;
}

Symbol SymbolTable::getVariable(const std::string& name)
{
    if (this->table.find(name) != this->table.end())
    {
        return this->table[name];
    }

    Symbol defaultSymbol;
    defaultSymbol.type = PrimitiveType::UINT16;
    defaultSymbol.value = uint16_t(0);

    return defaultSymbol;
}

bool SymbolTable::hasVariable(const std::string& name) const
{
	return this->table.find(name) != this->table.end();
}

size_t SymbolTable::getVariableCount() const
{
    return this->table.size();
}
#include "SymbolTable.h"
#include <unordered_map> 

void SymbolTable::setVariable(const std::string& name, PrimitiveType type, PrimitiveValue value)
{
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

#include "SymbolTable.h"
#include <unordered_map> 

void SymbolTable::setVariable(const std::string& name, PrimitiveType type, PrimitiveValue value)
{
	Symbol s = { type, value };
	this->table[name] = s;
}

PrimitiveValue SymbolTable::getVariable(const std::string& name)
{
    if (this->table.find(name) != this->table.end())
    {
        return this->table[name].value;
    }
    return uint16_t(0);
}

bool SymbolTable::hasVariable(const std::string& name) const
{
	return this->table.find(name) != this->table.end();
}

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H


#include "common.h"

#include <unordered_map>


class SymbolTable {
public:
    void add(const std::string& name, uint32_t addr);
    std::optional<uint32_t> get(const std::string& name) const;
    bool exists(const std::string& name) const;

private:
    std::unordered_map<std::string, uint32_t> symbols_;
};

#endif // SYMBOL_TABLE_H
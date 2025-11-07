#include "symbol_table.h"

#include <stdexcept>


void SymbolTable::add(const std::string& name, uint32_t addr) {
    if (symbols_.contains(name)) {
        throw std::runtime_error("Duplicate label: " + name);
    }
    symbols_[name] = addr;
}

std::optional<uint32_t> SymbolTable::get(const std::string& name) const {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) return it->second;
    return std::nullopt;
}

bool SymbolTable::exists(const std::string& name) const {
    return symbols_.contains(name);
}
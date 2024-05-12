#pragma once
#include <polyhook2/Exceptions/AVehHook.hpp>

namespace ModuleManager
{
    class SymbolResolver
    {        
    public:
        LPVOID dllBaseAddress;
        bool found_pdb;
        std::unordered_map<std::string, uint32_t> symbols;

        SymbolResolver();

        void* ResolveSymbol(const char* mangledSymbolName);
    };
}

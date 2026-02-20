#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#endif /* __PROGTEST__ */

struct Function{ //struct to store info about function
    std::string name;
    std::string sourceFile = "";
    uint32_t begin;
    uint32_t end;
    std::unordered_map<uint32_t, Function*> dependencies;
};

class CLinker
{
public:
    CLinker() = default;

    ~CLinker() = default;

    CLinker(const CLinker& other);

    CLinker& operator=(const CLinker& other);

    CLinker & addFile ( const std::string & fileName );

    void linkOutput ( const std::string & fileName,
                      const std::string & entryPoint );

private:
    std::unordered_map<std::string, std::unique_ptr<Function>> functions;
};

void dfs(Function* func, std::unordered_set<Function*>& visited, std::vector<Function*>& ordered) {
    if (visited.count(func)) return;
    visited.insert(func);
    for (const auto& [callSite, dep] : func->dependencies)
        dfs(dep, visited, ordered);

    ordered.push_back(func);  // post-order ensures dependencies come first
}

CLinker::CLinker(const CLinker& other) {
    std::unordered_map<const Function*, Function*> pointerMap;

    // сopy functions
    for (const auto& [name, funcPtr] : other.functions) {
        auto newFunc = std::make_unique<Function>(*funcPtr);
        pointerMap[funcPtr.get()] = newFunc.get();
        functions[name] = std::move(newFunc);
    }

    // fix dependencies
    for (auto& [_, funcPtr] : functions) {
        std::unordered_map<uint32_t, Function*> newDeps;
        for (const auto& [callAddr, dep] : funcPtr->dependencies) {
            newDeps[callAddr] = pointerMap.at(dep);
        }
        funcPtr->dependencies = std::move(newDeps);
    }
}

CLinker& CLinker::operator=(const CLinker& other) {
    if (this == &other)
        return *this;

    functions.clear();
    std::unordered_map<const Function*, Function*> pointerMap;

    for (const auto& [name, funcPtr] : other.functions) {
        auto newFunc = std::make_unique<Function>(*funcPtr);
        pointerMap[funcPtr.get()] = newFunc.get();
        functions[name] = std::move(newFunc);
    }

    for (auto& [_, funcPtr] : functions) {
        std::unordered_map<uint32_t, Function*> newDeps;
        for (const auto& [callAddr, dep] : funcPtr->dependencies) {
            newDeps[callAddr] = pointerMap.at(dep);
        }
        funcPtr->dependencies = std::move(newDeps);
    }

    return *this;
}

CLinker &CLinker::addFile(const std::string &fileName) {
    std::ifstream fin(fileName, std::ios::binary);
    if (!fin) throw std::runtime_error("Cannot open file");

    fin.seekg(0, std::ios::end);
    std::streamsize fileSize = fin.tellg();
    fin.seekg(0, std::ios::beg);
    //read header
    uint32_t exportCount, importCount, codeSize;
    fin.read(reinterpret_cast<char*>(&exportCount), sizeof(exportCount));
    fin.read(reinterpret_cast<char*>(&importCount), sizeof(importCount));
    fin.read(reinterpret_cast<char*>(&codeSize), sizeof(codeSize));

    if (!fin) throw std::runtime_error("Failed to read file header");


    // read export table into a temporary structure
    struct ExportEntry {
        std::string name;
        uint32_t offset;
    };
    std::vector<ExportEntry> exports;

    for (uint32_t i = 0; i < exportCount; ++i) {
        uint8_t nameLen;
        if (!fin.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)))
            throw std::runtime_error("Malformed export table: name length");

        std::string name(nameLen, '\0');
        if (!fin.read(name.data(), nameLen))
            throw std::runtime_error("Malformed export table: name");

        uint32_t offset;
        if (!fin.read(reinterpret_cast<char*>(&offset), sizeof(offset)))
            throw std::runtime_error("Malformed export table: offset");

        exports.push_back({name, offset});
    }

    // read import table into temporary structure
    struct ImportEntry {
        std::string name;
        std::vector<uint32_t> callSites;
    };
    std::vector<ImportEntry> imports;

    for (uint32_t i = 0; i < importCount; ++i) {
        uint8_t nameLen;
        if (!fin.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)))
            throw std::runtime_error("Malformed import table: name length");

        std::string name(nameLen, '\0');
        if (!fin.read(name.data(), nameLen))
            throw std::runtime_error("Malformed import table: name");

        uint32_t useCount;
        if (!fin.read(reinterpret_cast<char*>(&useCount), sizeof(useCount)))
            throw std::runtime_error("Malformed import table: use count");

        std::vector<uint32_t> callSites(useCount);

        for (uint32_t j = 0; j < useCount; ++j)
            if (!fin.read(reinterpret_cast<char*>(&callSites[j]), sizeof(callSites[j])))
                throw std::runtime_error("Malformed import table: call site");

        imports.push_back({name, std::move(callSites)});
    }

    // store codeStart
    uint32_t codeStart = static_cast<uint32_t>(fin.tellg());
    if (codeStart + codeSize > static_cast<uint32_t>(fileSize)) {
        throw std::runtime_error("Invalid file: code section exceeds file size");
    }

    // create Function objects for exports
    std::vector<std::pair<uint32_t, Function*>> functionsWithOffsets;

    for (const auto& exp : exports) {
        Function* funcPtr = nullptr;
        auto it = functions.find(exp.name);

        if (!functions.empty() && it != functions.end()) { //check whether functions was called before
            if (!it->second->sourceFile.empty())
                throw std::runtime_error("Duplicate symbol: " + exp.name);

            // already seen as an import, now we fill it
            it->second->sourceFile = fileName;
            it->second->begin = codeStart + exp.offset;
            funcPtr = it->second.get();
        } else {
            // new function
            auto func = std::make_unique<Function>();
            func->name = exp.name;
            func->sourceFile = fileName;
            func->begin = codeStart + exp.offset;
            funcPtr = func.get();
            functions[exp.name] = std::move(func);
        }

        functionsWithOffsets.emplace_back(funcPtr->begin, funcPtr);
    }

    // computing function ends
    std::sort(functionsWithOffsets.begin(), functionsWithOffsets.end());
    for (size_t i = 0; i + 1 < functionsWithOffsets.size(); ++i) {
        Function* curr = functionsWithOffsets[i].second;
        Function* next = functionsWithOffsets[i + 1].second;
        curr->end = next->begin - 1;
    }
    if (!functionsWithOffsets.empty()) {
        Function* last = functionsWithOffsets.back().second;
        last->end = codeStart + codeSize - 1;
    }

    // set up dependencies from imports
    for (const auto& imp : imports) {
        Function* importedFunc;
        auto it = functions.find(imp.name);
        if (it != functions.end())
            importedFunc = functions[imp.name].get();
        else {
            auto newFunc = std::make_unique<Function>();
            newFunc->name = imp.name;
            importedFunc = newFunc.get();
            functions[imp.name] = std::move(newFunc);
        }

        for (const auto& [startOffset, func] : functionsWithOffsets)
            for (uint32_t pos : imp.callSites) {
                uint32_t absPos = codeStart + pos;
                if (absPos >= func->begin && absPos <= func->end)
                    func->dependencies[absPos] = importedFunc;
            }
    }

    return *this;
}

void CLinker::linkOutput(const std::string &fileName, const std::string &entryPoint) {
    std::ofstream fout(fileName, std::ios::binary);
    if (!fout.is_open() || !fout)
        throw std::runtime_error("Failed to create output file: " + fileName);

    auto it = functions.find(entryPoint);
    if(it == functions.end())
        throw std::runtime_error("Failed to find function: " + entryPoint);
    else if(it->second->sourceFile == "")
        throw std::runtime_error("Function is only imported: " + entryPoint);

    //make a vector of all functions needed to be included in the output file
    Function* entryFunction = it->second.get();
    std::unordered_set<Function*> visited;
    std::vector<Function*> ordered;
    dfs(entryFunction, visited, ordered);
    std::reverse(ordered.begin(), ordered.end()); //reverse it so it starts from entry point

    //calculate offsets of functions in the new output file
    std::unordered_map<Function*, size_t> functionOffsets;
    size_t currentOffset = 0;
    for (Function* func : ordered) {
        functionOffsets[func] = currentOffset;
        currentOffset += func->end - func->begin + 1;
    }


    for (Function* func : ordered) {
        size_t codeSize = func->end - func->begin + 1;
        std::vector<uint8_t> patchedCode(codeSize); //stores original code of the function (with invalid calls of other functions)

        // read original code
        std::ifstream fin(func->sourceFile, std::ios::binary);
        if (!fin) throw std::runtime_error("Cannot open source file: " + func->sourceFile);
        fin.seekg(func->begin);
        fin.read(reinterpret_cast<char*>(patchedCode.data()), codeSize);
        if (!fin) throw std::runtime_error("Failed to read code for function: " + func->name);


        // calculate new address for the function call
        for (const auto& [addrInFunc, target] : func->dependencies) {
            size_t relativeAddr = addrInFunc - func->begin;
            if (relativeAddr + 4 > codeSize) throw std::runtime_error("Call site out of bounds");


            uint32_t patchedAddr = static_cast<uint32_t>(functionOffsets.at(target));
            std::memcpy(&patchedCode[relativeAddr], &patchedAddr, sizeof(patchedAddr));
        }

        // write patched code to output
        fout.write(reinterpret_cast<char*>(patchedCode.data()), patchedCode.size());
        if (!fout) throw std::runtime_error("Failed to write function code to output file: " + func->name);

    }
}

#ifndef __PROGTEST__
int main ()
{

    CLinker () . addFile ( "0in0.o" ) . linkOutput ( "0out", "strlen" );

    CLinker () . addFile ( "1in0.o" ) . linkOutput ( "1out", "main" );

    CLinker () . addFile ( "2in0.o" ) . addFile ( "2in1.o" ) . linkOutput ( "2out", "main" );

    CLinker () . addFile ( "3in0.o" ) . addFile ( "3in1.o" ) . linkOutput ( "3out", "towersOfHanoi" );

    try
    {
        CLinker () . addFile ( "4in0.o" ) . addFile ( "4in1.o" ) . linkOutput ( "4out", "unusedFunc" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Undefined symbol qsort
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    try
    {
        CLinker () . addFile ( "5in0.o" ) . linkOutput ( "5out", "main" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Duplicate symbol: printf
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    try
    {
        CLinker () . addFile ( "6in0.o" ) . linkOutput ( "6out", "strlen" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Cannot read input file
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    try
    {
        CLinker () . addFile ( "7in0.o" ) . linkOutput ( "7out", "strlen2" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Undefined symbol strlen2
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    CLinker () . addFile ( "0010_0.o" ) . addFile ( "0010_1.o" ) . addFile ( "0010_2.o" ) . addFile ( "0010_3.o" ) . linkOutput ( "0010_out", "pdrolowjjgdwxiadj" );
    CLinker () . addFile ( "0011_0.o" ) . addFile ( "0011_1.o" ) . linkOutput ( "0011_out", "yntvlhvtp" );
    CLinker () . addFile ( "0012_0.o" ) . addFile ( "0012_1.o" ) . addFile ( "0012_2.o" ) . linkOutput ( "0012_out", "acnskqfuegem" );
    CLinker () . addFile ( "0013_0.o" ) . addFile ( "0013_1.o" ) . addFile ( "0013_2.o" ) . linkOutput ( "0013_out", "yvjbkannhcusuktuhl" );
    CLinker () . addFile ( "0014_0.o" ) . addFile ( "0014_1.o" ) . addFile ( "0014_2.o" ) . linkOutput ( "0014_out", "adqcwiahautvfi" );


    return EXIT_SUCCESS;

}
#endif /* __PROGTEST__ */


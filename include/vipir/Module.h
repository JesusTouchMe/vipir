// Copyright 2024 solar-mist

/*
 * A module represents a single translation unit(a file)
 * to be compiled using vipir
*/

#ifndef VIPIR_MODULE_H
#define VIPIR_MODULE_H 1

#include "vipir/IR/Global.h"
#include "vipir/IR/GlobalVar.h"

#include "vipir/ABI/ABI.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace vipir
{
    enum class OutputFormat
    {
        ELF,
        PE,
    };

    enum class Pass
    {
        PeepholeOptimization
    };

    class Module
    {
    using GlobalPtr = std::unique_ptr<Global>;
    using ValuePtr = std::unique_ptr<Value>;
    public:
        Module(std::string name);

        template <class T>
        void setABI()
        {
            assert(mAbi == nullptr);
            mAbi = std::make_unique<T>();
        }

        void addPass(Pass pass);

        abi::ABI* abi() const;
        std::string_view getName() const;
        int getNextValueId();

        GlobalVar* createGlobalVar(Type* type);
        void insertGlobal(Global* global);
        void insertGlobalAt(Global* global, int offset);
        void insertGlobalAtFront(Global* global);
        void insertConstant(Value* constant);

        void print(std::ostream& stream) const;
        void printLIR(std::ostream& stream) const;

        void emit(std::ostream& stream, OutputFormat outputFormat);

    private:
        std::string mName;
        std::unique_ptr<abi::ABI> mAbi;
        std::vector<GlobalPtr> mGlobals;
        std::vector<ValuePtr> mConstants;
        std::vector<Pass> mPasses;
    };

    Value* getPointerOperand(Value* value);
}

#endif // VIPIR_MODULE_H
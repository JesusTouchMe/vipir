// Copyright 2023 solar-mist


#include "vipir/Module.h"

#include "vipir/IR/Instruction/LoadInst.h"

#include "vipir/IR/Function.h"

#include "vipir/MC/Builder.h"

#include "vipir/Optimizer/RegAlloc/RegAlloc.h"
#include "vipir/Optimizer/Peephole/Peephole.h"

#include "vasm/codegen/Elf.h"
#include "vasm/codegen/Pe.h"
#include "vasm/codegen/IOutputFormat.h"
#include "vasm/codegen/builder/OpcodeBuilder.h"

#include <algorithm>
#include <sstream>
#include <format>

namespace vipir
{
    Module::Module(std::string name)
        :mName(std::move(name))
    {
    }

    void Module::addPass(Pass pass)
    {
        mPasses.push_back(pass);
    }

    abi::ABI* Module::abi() const
    {
        return mAbi.get();
    }

    std::string_view Module::getName() const
    {
        return mName;
    }

    int Module::getNextValueId()
    {
        static int valueId = 0;
        return ++valueId;
    }

    GlobalVar* Module::createGlobalVar(Type* type)
    {
        GlobalVar* global = new GlobalVar(*this, type);
        insertGlobalAt(global, -1);
        return global;
    }

    void Module::insertGlobal(Global* global)
    {
        mGlobals.push_back(GlobalPtr(global));
    }

    void Module::insertGlobalAt(Global* global, int offset)
    {
        if (mGlobals.empty())
        {
            mGlobals.push_back(GlobalPtr(global));
        }
        else
        {
            mGlobals.insert(mGlobals.end() + offset, GlobalPtr(global));
        }
    }

    void Module::insertGlobalAtFront(Global* global)
    {
        mGlobals.insert(mGlobals.begin(), GlobalPtr(global));
    }

    void Module::insertConstant(Value* constant)
    {
        mConstants.push_back(ValuePtr(constant));
    }

    void Module::print(std::ostream& stream) const
    {
        stream << std::format("file \"{}\"", mName);

        for (const GlobalPtr& global : mGlobals)
        {
            global->print(stream);
        }
    }


    void Module::printLIR(std::ostream& stream) const
    {
        opt::RegAlloc regalloc;
        for (const GlobalPtr& global : mGlobals)
        {
            if (auto func = dynamic_cast<Function*>(global.get()))
            {
                regalloc.assignVirtualRegisters(func, mAbi.get());
            }
        }

        for (const GlobalPtr& global : mGlobals)
        {
            if (auto func = dynamic_cast<Function*>(global.get()))
            {
                func->setEmittedValue();
            }
        }

        lir::Builder builder;
        for (const ValuePtr& constant : mConstants)
        {
            constant->emit(builder);
        }
        for (const GlobalPtr& global : mGlobals)
        {
            global->emit(builder);
        }

        if (std::find(mPasses.begin(), mPasses.end(), Pass::PeepholeOptimization) != mPasses.end())
        {
            opt::Peephole peephole;
            peephole.doOptimizations(builder);
        }

        for (auto& value : builder.getValues())
        {
            value->print(stream);
        }
    }

    void Module::emit(std::ostream& stream, OutputFormat format)
    {
        opt::RegAlloc regalloc;
        for (const GlobalPtr& global : mGlobals)
        {
            if (auto func = dynamic_cast<Function*>(global.get()))
            {
                regalloc.assignVirtualRegisters(func, mAbi.get());
            }
        }

        lir::Builder builder;
        for (const ValuePtr& constant : mConstants)
        {
            constant->emit(builder);
        }

        for (GlobalPtr& global : mGlobals)
        {
            if (auto func = dynamic_cast<Function*>(global.get()))
            {
                func->setEmittedValue();
            }
        }

        for (const GlobalPtr& global : mGlobals)
        {
            global->emit(builder);
        }

        if (std::find(mPasses.begin(), mPasses.end(), Pass::PeepholeOptimization) != mPasses.end())
        {
            opt::Peephole peephole;
            peephole.doOptimizations(builder);
        }

        for (auto& global : mGlobals)
        {
            if (auto func = dynamic_cast<Function*>(global.get()))
            {
                func->setCalleeSaved();
            }
        }

        MC::Builder mcBuilder;
        for (auto& value : builder.getValues())
        {
            value->emit(mcBuilder);
        }

        std::unique_ptr<codegen::IOutputFormat> outputFormat;
        switch(format)
        {
            case OutputFormat::ELF:
                outputFormat = std::make_unique<codegen::ELFFormat>(mName);
                break;
            case OutputFormat::PE:
                outputFormat = std::make_unique<codegen::PEFormat>(mName);
                break;
        }

        codegen::OpcodeBuilder opcodeBuilder = codegen::OpcodeBuilder(outputFormat.get(), mName);

        for (const auto& value : mcBuilder.getValues())
        {
            value->emit(opcodeBuilder, opcodeBuilder.getSection());
        }

        opcodeBuilder.patchForwardLabels();

        outputFormat->print(stream);
    }


    Value* getPointerOperand(Value* value)
    {
        if (LoadInst* load = dynamic_cast<LoadInst*>(value))
        {
            return load->getPointer();
        }

        return nullptr;
    }
}
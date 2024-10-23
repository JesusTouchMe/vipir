// Copyright 2024 solar-mist

#include "vipir/IR/Instruction/AddrInst.h"
#include "vipir/IR/Instruction/AllocaInst.h"

#include "vipir/IR/BasicBlock.h"
#include "vipir/IR/Function.h"

#include "vipir/Module.h"

#include "vipir/LIR/Instruction/LoadAddress.h"

#include "vasm/instruction/operand/Register.h"
#include "vasm/instruction/operand/Relative.h"
#include "vasm/instruction/operand/Label.h"

#include "vasm/instruction/twoOperandInstruction/LeaInstruction.h"

#include <cassert>
#include <format>

namespace vipir
{
    void AddrInst::print(std::ostream& stream)
    {
        stream << std::format("addr {} %{}, {}", mType->getName(), getName(mValueId), mPtr->ident());
    }

    Value* AddrInst::getPointer()
    {
        return mPtr;
    }

    std::string AddrInst::ident() const
    {
        return std::format("%{}", getName(mValueId));
    }

    std::vector<Value*> AddrInst::getOperands()
    {
        return { mPtr };
    }

    void AddrInst::doConstantFold()
    {
    }

    void AddrInst::emit(lir::Builder& builder)
    {
        mPtr->lateEmit(builder);

        lir::OperandPtr ptr = mPtr->getEmittedValue();
        mEmittedValue = std::move(ptr);
    }

    AddrInst::AddrInst(BasicBlock* parent, Value* ptr)
        : Instruction(parent->getModule(), parent)
        , mPtr(ptr)
        , mValueId(mModule.getNextValueId())
    {
        mType = mPtr->getType();
        mRequiresVReg = false;

        if (auto alloca = dynamic_cast<AllocaInst*>(mPtr))
        {
            alloca->forceMemory();
        }
        if (auto func = dynamic_cast<Function*>(mPtr))
        {
            mType = Type::GetPointerType(mType);
        }
    }
}
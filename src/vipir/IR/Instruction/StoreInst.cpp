// Copyright 2024 solar-mist

#include "vipir/IR/Instruction/StoreInst.h"
#include "vipir/IR/Instruction/LoadInst.h"
#include "vipir/IR/Instruction/GEPInst.h"

#include "vipir/IR/BasicBlock.h"
#include "vipir/IR/GlobalVar.h"

#include "vipir/LIR/Instruction/Move.h"

#include "vasm/instruction/operand/Register.h"
#include "vasm/instruction/operand/Memory.h"
#include "vasm/instruction/operand/Label.h"
#include "vasm/instruction/operand/Relative.h"

#include "vasm/instruction/twoOperandInstruction/MovInstruction.h"
#include "vasm/instruction/twoOperandInstruction/LeaInstruction.h"
#include <format>

namespace vipir
{
    void StoreInst::print(std::ostream& stream)
    {
        stream << std::format("store {} -> {}", mValue->ident(), mPtr->ident());
    }

    std::vector<Value*> StoreInst::getOperands()
    {
        return {mPtr, mValue};
    }

    std::string StoreInst::ident() const
    {
        return "%undef";
    }

    void StoreInst::doConstantFold()
    {
    }


    void StoreInst::emit(lir::Builder& builder)
    {
        mPtr->lateEmit(builder);
        mValue->lateEmit(builder);

        lir::OperandPtr ptr = mPtr->getEmittedValue();
        lir::OperandPtr value = mValue->getEmittedValue();

        if (dynamic_cast<LoadInst*>(mPtr))
        {
            ptr = std::make_unique<lir::Memory>(mValue->getType()->getOperandSize(), std::move(ptr), std::nullopt, nullptr, std::nullopt);
        }
        else if (dynamic_cast<GEPInst*>(mPtr))
        {
            ptr = std::make_unique<lir::Memory>(mValue->getType()->getOperandSize(), std::move(ptr), std::nullopt, nullptr, std::nullopt);
        }

        builder.addValue(std::make_unique<lir::Move>(std::move(ptr), std::move(value)));
    }

    StoreInst::StoreInst(BasicBlock* parent, Value* ptr, Value* value)
        : Instruction(parent->getModule(), parent)
        , mPtr(ptr)
        , mValue(value)
    {
        mRequiresVReg = false;
        if (dynamic_cast<GlobalVar*>(mPtr) && mValue->isConstant() && mValue->getType()->getOperandSize() == codegen::OperandSize::Quad)
        {
            mRequiresVReg = true;
        }
    }
}
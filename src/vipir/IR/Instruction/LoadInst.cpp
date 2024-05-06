// Copyright 2024 solar-mist

#include "vipir/IR/Instruction/LoadInst.h"
#include "vipir/IR/Instruction/AllocaInst.h"

#include "vipir/IR/BasicBlock.h"

#include "vipir/LIR/Instruction/Move.h"
#include "vipir/Module.h"

#include "vasm/instruction/twoOperandInstruction/MovInstruction.h"

#include "vasm/instruction/operand/Memory.h"
#include "vasm/instruction/operand/Register.h"
#include "vasm/instruction/operand/Label.h"
#include "vasm/instruction/operand/Relative.h"

#include "vipir/Type/PointerType.h"

#include <cassert>
#include <format>

namespace vipir
{
    void LoadInst::print(std::ostream& stream)
    {
        stream << std::format("load {} %{}, {}", mType->getName(), getName(mValueId), mPtr->ident());
    }

    Value* LoadInst::getPointer()
    {
        return mPtr;
    }
    std::vector<Value*> LoadInst::getOperands()
    {
        return {mPtr};
    }

    std::string LoadInst::ident() const
    {
        return std::format("%{}", getName(mValueId));
    }

    void LoadInst::emit(MC::Builder& builder)
    {
        codegen::OperandSize size = mType->getOperandSize();
        
        instruction::OperandPtr ptr = mPtr->getEmittedValue();
        instruction::OperandPtr operand = mVReg->operand(size);

        if (dynamic_cast<AllocaInst*>(mPtr))
        {
            if (dynamic_cast<instruction::Memory*>(ptr.get()))
            {
                if (dynamic_cast<instruction::Memory*>(operand.get()))
                {
                    instruction::RegisterPtr temp = std::make_unique<instruction::Register>(3, mType->getOperandSize());
                    builder.addValue(std::make_unique<instruction::MovInstruction>(temp->clone(), std::move(ptr)));
                    ptr = std::move(temp);
                }
                builder.addValue(std::make_unique<instruction::MovInstruction>(operand->clone(), std::move(ptr)));
                mEmittedValue = std::move(operand);
            }
            else
            {
                mEmittedValue = std::move(ptr);
            }
        }
        else
        {
            if (dynamic_cast<instruction::Memory*>(ptr.get()) || dynamic_cast<instruction::Relative*>(ptr.get()))
            {
                if (dynamic_cast<instruction::Memory*>(operand.get()))
                {
                    instruction::RegisterPtr temp = std::make_unique<instruction::Register>(3, mType->getOperandSize());
                    builder.addValue(std::make_unique<instruction::MovInstruction>(temp->clone(), std::move(ptr)));
                    ptr = std::move(temp);
                }
                builder.addValue(std::make_unique<instruction::MovInstruction>(operand->clone(), std::move(ptr)));
            }
            else if (auto regOperand = dynamic_cast<instruction::Register*>(ptr.get()))
            {
                (void)ptr.release();
                instruction::RegisterPtr ptrReg = instruction::RegisterPtr(regOperand);
                instruction::OperandPtr memory = std::make_unique<instruction::Memory>(std::move(ptrReg), std::nullopt, nullptr, std::nullopt);
                if (dynamic_cast<instruction::Memory*>(operand.get()))
                {
                    instruction::RegisterPtr temp = std::make_unique<instruction::Register>(3, mType->getOperandSize());
                    builder.addValue(std::make_unique<instruction::MovInstruction>(temp->clone(), std::move(memory)));
                    memory = std::move(temp);
                }
                builder.addValue(std::make_unique<instruction::MovInstruction>(operand->clone(), std::move(memory)));
            }
            else if (auto labelOperand = dynamic_cast<instruction::LabelOperand*>(ptr.get()))
            {
                (void)ptr.release();
                instruction::LabelOperandPtr labelPtr = instruction::LabelOperandPtr(labelOperand);
                instruction::RelativePtr rel = std::make_unique<instruction::Relative>(std::move(labelPtr), std::nullopt);
                builder.addValue(std::make_unique<instruction::MovInstruction>(operand->clone(), std::move(rel)));
            }
            mEmittedValue = std::move(operand);
        }
    }

    void LoadInst::emit2(lir::Builder& builder)
    {
        lir::OperandPtr vreg = std::make_unique<lir::VirtualReg>(mVReg, mType->getOperandSize());
        builder.addValue(std::make_unique<lir::Move>(vreg->clone(), mPtr->getEmittedValue2()));
        mEmittedValue2 = std::move(vreg);
    }

    LoadInst::LoadInst(BasicBlock* parent, Value* ptr)
        : Instruction(parent->getModule(), parent)
        , mPtr(ptr)
        , mValueId(mModule.getNextValueId())
    {
        assert(mPtr->getType()->isPointerType());
        mType = static_cast<PointerType*>(mPtr->getType())->getBaseType();
    }
}
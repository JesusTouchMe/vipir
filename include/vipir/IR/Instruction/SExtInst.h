// Copyright 2024 solar-mist

/*
 * An instruction that sign extends an integral
 * value to a larger integral type
*/

#ifndef VIPIR_IR_INSTRUCTION_SEXT_INST_H
#define VIPIR_IR_INSTRUCTION_SEXT_INST_H 1

#include "vipir/IR/Instruction/Instruction.h"

namespace vipir
{
    class SExtInst : public Instruction
    {
    friend class IRBuilder;
    public:
        void print(std::ostream& stream) override;
        std::string ident() const override;
    
        std::vector<Value*> getOperands() override;

    protected:
        void emit(MC::Builder& builder) override;

    private:
        SExtInst(BasicBlock* parent, Value* value, Type* destType);

        Value* mValue;

        int mValueId;
    };
}

#endif // VIPIR_IR_INSTRUCTION_SEXT_INST_H
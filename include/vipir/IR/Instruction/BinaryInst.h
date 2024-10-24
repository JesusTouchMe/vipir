// Copyright 2024 solar-mist

/*
 * An instruction that executes a binary operation
 * such as adding or subtracting
*/

#ifndef VIPIR_IR_INSTRUCTION_BINARY_INST_H
#define VIPIR_IR_INSTRUCTION_BINARY_INST_H 1

#include "vipir/IR/Instruction/Instruction.h"

namespace vipir
{
    class BinaryInst : public Instruction
    {
    friend class IRBuilder;
    public:
        void print(std::ostream& stream) override;
        std::string ident() const override;

        void doConstantFold() override;
    
        std::vector<std::reference_wrapper<Value*> > getOperands() override;
        std::vector<int> getRegisterSmashes() override;

    protected:
        void emit(lir::Builder& builder) override;
        void lateEmit(lir::Builder& builder) override;

    private:
        BinaryInst(BasicBlock* parent, Value* left, Instruction::BinaryOperators op, Value* right);

        Value* mLeft;
        Instruction::BinaryOperators mOperator;
        Value* mRight;

        int mValueId;

        MC::Builder* mBuilder; // For comparison operations that are emitted when used
    };
}

#endif // VIPIR_IR_INSTRUCTION_BINARY_INST_H
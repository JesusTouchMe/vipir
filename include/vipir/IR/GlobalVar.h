// Copyright 2024 solar-mist

/*
 * GlobalVars are a value in memory
 * that exist in the global scope
*/

#ifndef VIPIR_IR_GLOBAL_VAR_H
#define VIPIR_IR_GLOBAL_VAR_H 1

#include "vipir/IR/Global.h"

namespace vipir
{
    class GlobalVar : public Global
    {
    friend class Module;
    public:
        void print(std::ostream& stream) override;
        std::string ident() const override;

        void doConstantFold() override;

        void setInitialValue(Value* value);

    protected:
        void emit(lir::Builder& builder) override;

        void emitConstant2(lir::Builder& builder, Type* type, lir::OperandPtr value);

    private:
        GlobalVar(Module& module, Type* type);

        Value* mInitialValue;
        int mValueId;
    };
}

#endif // VIPIR_IR_GLOBAL_VAR_H
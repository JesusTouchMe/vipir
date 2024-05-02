
#include "vipir/IR/Constant/ConstantStruct.h"
#include "vipir/Module.h"

#include "vipir/IR/Function.h"
#include "vipir/IR/GlobalVar.h"
#include "vipir/IR/BasicBlock.h"
#include "vipir/IR/IRBuilder.h"
#include "vipir/IR/Constant/ConstantInt.h"
#include "vipir/IR/Instruction/AllocaInst.h"
#include "vipir/IR/Instruction/AddrInst.h"
#include "vipir/IR/Instruction/GEPInst.h"
#include "vipir/IR/Instruction/LoadInst.h"

#include "vipir/Type/StructType.h"
#include "vipir/Type/ArrayType.h"
#include "vipir/Type/Type.h"

#include "vipir/ABI/SysV.h"

#include <iostream>
#include <fstream>

int main()
{
    vipir::Module mod("test.tst");
    mod.setABI<vipir::abi::SysV>();

    vipir::IRBuilder builder;

    auto i32Type = vipir::Type::GetIntegerType(32);
    auto i64Type = vipir::Type::GetIntegerType(64);
    auto i8Type = vipir::Type::GetIntegerType(8);
    auto arrayType = vipir::Type::GetArrayType(i32Type, 3);

    auto func2 = vipir::Function::Create(vipir::FunctionType::Create(i32Type, {i32Type}), mod, "main");
    auto bb2 = vipir::BasicBlock::Create("", func2);

    builder.setInsertPoint(bb2);

    auto alloca = builder.CreateAlloca(arrayType, "");
    auto val = vipir::ConstantInt::Get(mod, 44, i32Type);
    auto gep = builder.CreateGEP(alloca, func2->getArgument(0));
    builder.CreateStore(gep, val);

    auto gep2 = builder.CreateGEP(alloca, func2->getArgument(0));
    auto ret = builder.CreateLoad(gep2);

    builder.CreateRet(ret);

    mod.emit(std::cout, vipir::OutputFormat::ELF);
}
#include "vipir/IR/GlobalString.h"
#include "vipir/Module.h"

#include "vipir/IR/Function.h"
#include "vipir/IR/GlobalVar.h"
#include "vipir/IR/BasicBlock.h"
#include "vipir/IR/IRBuilder.h"
#include "vipir/IR/Constant/ConstantInt.h"
#include "vipir/IR/Constant/ConstantStruct.h"
#include "vipir/IR/Constant/ConstantArray.h"
#include "vipir/IR/Instruction/AllocaInst.h"
#include "vipir/IR/Instruction/AddrInst.h"
#include "vipir/IR/Instruction/CallInst.h"
#include "vipir/IR/Instruction/BinaryInst.h"
#include "vipir/IR/Instruction/LoadInst.h"
#include "vipir/IR/Instruction/GEPInst.h"

#include "vipir/Optimizer/RegAlloc/RegAlloc.h"
#include "vipir/Type/FunctionType.h"
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
    auto voidType = vipir::Type::GetVoidType();
    auto structType = vipir::Type::GetStructType({i32Type, i32Type});

    auto func1 = vipir::Function::Create(vipir::FunctionType::Create(i32Type, {i32Type}), mod, "main");
    auto bb1 = vipir::BasicBlock::Create("", func1);

    builder.setInsertPoint(bb1);

    auto alloca = builder.CreateAlloca(structType);
    auto gep1 = builder.CreateStructGEP(alloca, 1);
    auto sto = vipir::ConstantInt::Get(mod, 23, i32Type);
    builder.CreateStore(gep1, sto);

    auto gep2 = builder.CreateStructGEP(alloca, 1);
    auto retval = builder.CreateLoad(gep2);

    builder.CreateRet(retval);

    mod.addPass(vipir::Pass::PeepholeOptimization);

    //mod.print2(std::cout);

    mod.emit2(std::cout, vipir::OutputFormat::ELF);
}
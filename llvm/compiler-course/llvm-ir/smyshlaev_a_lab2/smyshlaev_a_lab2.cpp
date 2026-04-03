#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

namespace {
struct MulShiftReplacePass : llvm::PassInfoMixin<MulShiftReplacePass> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    bool Changed = false;
    for (auto &bb : func)
    {
      for (auto &instr: llvm::make_early_inc_range(bb))
      {
        if(auto* BinOp = llvm::dyn_cast<llvm::BinaryOperator>(&instr))
        {
          unsigned opCode = BinOp->getOpcode();
          if(opCode != llvm::Instruction::Mul && opCode != llvm::Instruction::UDiv && opCode != llvm::Instruction::SDiv) continue;
          auto* lhs = BinOp->getOperand(0);
          auto* rhs = BinOp->getOperand(1);
          auto* f_constant = llvm::dyn_cast<llvm::ConstantInt>(lhs);
          auto* s_constant = llvm::dyn_cast<llvm::ConstantInt>(rhs);
          if (!(f_constant || s_constant)) continue;
          llvm::IRBuilder<> builder(BinOp);
          if (f_constant)
          {
            if(f_constant->getValue().isPowerOf2())
            {
              uint64_t shift = f_constant->getValue().logBase2();
              auto* shiftConst = llvm::ConstantInt::get(BinOp->getType(), shift);
              if (opCode == llvm::Instruction::Mul)
              {
                auto* new_instr = builder.CreateShl(rhs, shiftConst);
                BinOp->replaceAllUsesWith(new_instr);
                BinOp->eraseFromParent();
                Changed = true;
                continue;
              }
            }
          }
          if(s_constant)
          {
            if(s_constant->getValue().isPowerOf2())
            {
              llvm::Value* new_instr = nullptr; 
              uint64_t shift = s_constant->getValue().logBase2();
              auto* shiftConst = llvm::ConstantInt::get(BinOp->getType(), shift);
              if (opCode == llvm::Instruction::Mul)
                new_instr = builder.CreateShl(lhs, shiftConst);
              else if (opCode == llvm::Instruction::UDiv)
                new_instr = builder.CreateLShr(lhs, shiftConst);
              else if (opCode == llvm::Instruction::SDiv)
              {
                if(BinOp->isExact()) {
                  new_instr = builder.CreateAShr(lhs, shiftConst);
                } 
                else {
                  uint64_t mask_val = (1ULL << shift) - 1;
                  auto* maskConst = llvm::ConstantInt::get(BinOp->getType(), mask_val);
                  auto* zeroConst = llvm::ConstantInt::get(BinOp->getType(), 0);
                  auto* isNeg = builder.CreateICmpSLT(lhs, zeroConst);
                  auto* adjustedLhs = builder.CreateAdd(lhs, maskConst);
                  auto* selectedVal = builder.CreateSelect(isNeg, adjustedLhs, lhs);
                  new_instr = builder.CreateAShr(selectedVal, shiftConst);
                }
              }
              if(new_instr)
              {
                BinOp->replaceAllUsesWith(new_instr);
                BinOp->eraseFromParent();
                Changed = true;
              }
            }
          }
        }
      }
    }
    return Changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MulShiftReplacePass", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "mulshift") {
                    FPM.addPass(MulShiftReplacePass{});
                    return true;
                  }
                  return false;
                });
          }};
}

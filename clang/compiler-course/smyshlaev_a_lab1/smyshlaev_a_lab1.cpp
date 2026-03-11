#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class SmyshlaevAVisitor final : public clang::RecursiveASTVisitor<SmyshlaevAVisitor> {
public:
  explicit SmyshlaevAVisitor(clang::ASTContext *context) : m_context(context) {}
  bool VisitFunctionDecl(clang::FunctionDecl *func) {
    func->dump();
    return true;
  }

private:
  clang::ASTContext *m_context;
};

class SmyshlaevAConsumer final : public clang::ASTConsumer {
public:
  explicit SmyshlaevAConsumer(clang::ASTContext *context) : m_visitor(context) {}

  void HandleTranslationUnit(clang::ASTContext &context) override {
    m_visitor.TraverseDecl(context.getTranslationUnitDecl());
  }

private:
  SmyshlaevAVisitor m_visitor;
};

class SmyshlaevAAction final : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &ci, llvm::StringRef) override {
    return std::make_unique<SmyshlaevAConsumer>(&ci.getASTContext());
  }

  bool ParseArgs(const clang::CompilerInstance &ci,
                 const std::vector<std::string> &args) override {
    return true;
  }
};
} // namespace

static clang::FrontendPluginRegistry::Add<SmyshlaevAAction>
    X("smyshlaev_a_lab1_plugin", "Description plugin");

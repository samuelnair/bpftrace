#include "ast/pass_manager.h"

#include <ostream>

#include "ast/passes/printer.h"
#include "bpftrace.h"

namespace bpftrace::ast {

namespace {
void print(Node *root, const std::string &name, std::ostream &out)
{
  out << "\nAST after: " << name << std::endl;
  out << "-------------------\n";
  ast::Printer printer(out);
  printer.print(root);
  out << std::endl;
}
} // namespace

void PassManager::AddPass(Pass p)
{
  passes_.push_back(std::move(p));
}

PassResult PassManager::Run(Node *root, PassContext &ctx)
{
  if (bt_debug.find(DebugStage::Ast) != bt_debug.end())
    print(root, "parser", std::cout);
  for (auto &pass : passes_) {
    auto result = pass.Run(*root, ctx);
    if (result.Root())
      root = result.Root();

    if (bt_debug.find(DebugStage::Ast) != bt_debug.end())
      print(root, pass.name, std::cout);

    if (!result.Ok())
      return result;
  }
  return PassResult::Success(root);
}

PassResult PassResult::Error(const std::string &pass)
{
  return PassResult(pass);
}

PassResult PassResult::Error(const std::string &pass, int code)
{
  return PassResult(pass, code);
}

PassResult PassResult::Error(const std::string &pass, const std::string &msg)
{
  return PassResult(pass, msg);
}

PassResult PassResult::Success(Node *root)
{
  return PassResult(root);
}

} // namespace bpftrace::ast

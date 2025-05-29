// ast.hh
#pragma once
#include <memory>
#include <string>
#include <vector>

struct AstNode
{
};

struct Expr : AstNode
{
        virtual ~Expr() = default;
};

struct IntExpr : Expr
{
        int value;

        IntExpr(int v) : value(v) { }
};

struct IdentExpr : Expr
{
        std::string name;

        IdentExpr(const std::string & s) : name(s) { }
};

struct MemberAccessExpr : Expr
{
        std::unique_ptr<IdentExpr> base;
        std::string field;

        MemberAccessExpr(std::unique_ptr<IdentExpr> b, const std::string & f)
            : base(std::move(b)), field(f)
        {
        }
};

struct Statement : AstNode
{
};
struct ModuleDecl : Statement
{
// TODO: module declarations that don't suck
};

struct Prog : AstNode
{
        std::vector<std::unique_ptr<Statement>> stmts;
};

struct FnArgLike : AstNode
{
        std::vector<std::unique_ptr<Expr>> args;
};

struct SystemTask : Statement
{
        std::string name;
        std::unique_ptr<FnArgLike> args;
};


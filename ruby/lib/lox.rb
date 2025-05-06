# frozen_string_literal: true

module Lox
  autoload :AstPrinter, "lox/ast_printer"
  autoload :CLI,        "lox/cli"
  autoload :Errors,     "lox/errors"
  autoload :Scanner,    "lox/scanner"
  autoload :Expr,       "lox/expr"
  autoload :Token,      "lox/token"
end

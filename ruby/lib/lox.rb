# frozen_string_literal: true

require_relative "lox/errors"

module Lox
  autoload :AstPrinter,  "lox/ast_printer"
  autoload :CLI,         "lox/cli"
  autoload :Interpreter, "lox/interpreter"
  autoload :Parser,      "lox/parser"
  autoload :Scanner,     "lox/scanner"
  autoload :Expr,        "lox/expr"
  autoload :Token,       "lox/token"
end

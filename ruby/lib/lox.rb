# frozen_string_literal: true

require_relative "lox/errors"

module Lox
  autoload :AstPrinter,  "lox/ast_printer"
  autoload :CLI,         "lox/cli"
  autoload :Environment, "lox/environment"
  autoload :Expr,        "lox/expr"
  autoload :Interpreter, "lox/interpreter"
  autoload :Parser,      "lox/parser"
  autoload :Scanner,     "lox/scanner"
  autoload :Stmt,        "lox/stmt"
  autoload :Token,       "lox/token"

  DEF_DIR = File.expand_path("../../defs", __dir__)

  def self.defs(name)
    File.read(File.join(Lox::DEF_DIR, "#{name}.def")).lines.map do |line|
      next if line.match?(/^\s*(#.*)?$/)

      next line.strip if line.match?(/^\w+$/)

      type, args = line.split(":", 2).map(&:strip)
      args = args.split(",").map(&:strip).map { |x| x.split(" ", 2) }
      [type, args]
    end.compact
  end
end

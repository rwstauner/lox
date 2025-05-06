module Lox
  class AstPrinter
    def initialize(expr)
      @expr = expr
    end

    def to_s
      @expr.accept(self)
    end

    def parenthesize(name, *items)
      "(#{name} #{items.map { |i| i.accept(self) }.join(" ")})"
    end

    def visit_binary(expr)
      parenthesize(expr.operator.lexeme, expr.left, expr.right)
    end

    def visit_grouping(expr)
      parenthesize("group", expr.expression)
    end

    def visit_literal(expr)
      expr.value
    end

    def visit_unary(expr)
      parenthesize(expr.operator.lexeme, expr.right)
    end
  end
end

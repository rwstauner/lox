
require_relative "support"

module Lox
  class AstPrinterTest < Minitest::Test
    def test_parenthesize
      str = AstPrinter.new(
        Expr::Binary.new(
          Expr::Unary.new(
            Token.new(Token::MINUS, "-", nil, 1),
            Expr::Literal.new(123)
          ),
          Token.new(Token::STAR, "*", nil, 1),
          Expr::Grouping.new(
            Expr::Literal.new(45.67)
          )
        )
      ).to_s
      assert_equal("(* (- 123) (group 45.67))", str)
    end
  end
end

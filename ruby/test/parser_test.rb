require_relative "support"

class ParserTest < Minitest::Test
  def test_expressions
    exprs = parse('true; false; 1; 2 + 3;')

    assert_equal([Lox::Stmt::Expression], exprs.map(&:class).uniq)

    assert_equal(true, exprs[0].expression.value)
    assert_equal(false, exprs[1].expression.value)
    assert_equal(1.0, exprs[2].expression.value)

    assert_equal(2.0, exprs[3].expression.left.value)
    assert_equal("+", exprs[3].expression.operator.lexeme)
    assert_equal(3.0, exprs[3].expression.right.value)
  end

  def test_statements
    stmts = parse('var abc = "foo"; print abc;')

    assert_equal(2, stmts.count)

    assert_equal(Lox::Stmt::Var, stmts[0].class)
    assert_equal("abc", stmts[0].name.lexeme)
    assert_equal("foo", stmts[0].initializer.value)

    assert_equal(Lox::Stmt::Print, stmts[1].class)
    assert_predicate(stmts[1].expression.name, :identifier?)
    assert_equal("abc", stmts[1].expression.name.lexeme)
  end

  private

  def parse(str)
    Lox::Parser.new(Lox::Scanner.new(str).scan.tokens).parse
  end
end

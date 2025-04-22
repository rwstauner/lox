require_relative "support"

class ScannerTest < Minitest::Test
  def test_tokens
    tokens = tokenize("var x_y = 10; x_y >= 20").map(&:type)
    assert_equal(
      %i[
        var
        identifier
        equal
        number
        semicolon

        identifier
        greater_equal
        number
        eof
      ],
      tokens,
    )
  end

  def test_identifier
    tokens = tokenize("var x_y = 10").select(&:identifier?)

    assert_equal(1, tokens.size)
    assert_equal("x_y", tokens.first.lexeme)
  end

  private

  def tokenize(str)
    Lox::Scanner.new(str).scan.tokens
  end
end

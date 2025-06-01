# frozen_string_literal: true

module Lox
  class Parser
    Error = Class.new(::RuntimeError)

    def initialize(tokens)
      @tokens = tokens
      @current = 0
    end

    def advance
      @current += 1
      previous
    end

    def peek
      @tokens[@current]
    end

    def previous
      raise if @current.zero?
      @tokens[@current - 1]
    end

    def eof?
      return true if @current >= @tokens.size
      peek.type == Token::EOF
    end

    def current?(type)
      return false if eof?
      peek.type == type
    end

    def match?(*types)
      types.each do |type|
        if current?(type)
          advance
          return true
        end
      end

      false
    end

    def consume(type, msg)
      if match?(type)
        return previous
      end
      error(peek, msg)
    end

    def error(token, msg)
      Lox.error(msg, token:)
      raise Error.new
    end

    STATEMENT_STARTERS = [
      Token::CLASS,
      Token::FUN,
      Token::VAR,
      Token::FOR,
      Token::IF,
      Token::WHILE,
      Token::PRINT,
      Token::RETURN,
    ]

    def synchronize
      advance

      while !eof?
        # If we just ended a statement this is a good spot.
        return if previous.type == Token::SEMICOLON

        return if STATEMENT_STARTERS.include?(peek.type)
      end

      advance
    end

    def parse
      statements = []
      while !eof?
        statements << declaration
      end
      statements
    rescue Error
      nil
    end

    # Grammar rules in increasing order of precedence.

    def primary
      return Expr::Literal.new(false) if match?(Token::FALSE)
      return Expr::Literal.new(true) if match?(Token::TRUE)
      return Expr::Literal.new(nil) if match?(Token::NIL)

      if match?(Token::NUMBER, Token::STRING)
        return Expr::Literal.new(previous.literal)
      end

      if match?(Token::IDENTIFIER)
        return Expr::Variable.new(previous)
      end

      if match?(Token::LEFT_PAREN)
        expr = expression
        consume(Token::RIGHT_PAREN, "Expect ')' after expression.")
        return Expr::Grouping.new(expr)
      end

      raise error(peek, "Expect expression")
    end

    def unary
      if match?(Token::BANG, Token::MINUS)
        operator = previous
        right = unary
        return Expr::Unary.new(operator, right)
      end

      primary
    end

    def self.binary(name, next_item, types)
      class_eval <<-CODE
        def #{name}
          expr = #{next_item}

          while match?(#{types.map { |t| "Token::#{t}" }.join(", ")})
            operator = previous
            right = #{next_item}
            expr = Expr::Binary.new(expr, operator, right)
          end

          expr
        end
      CODE
    end

    binary :factor, :unary, %i[SLASH STAR]
    binary :term, :factor, %i[MINUS PLUS]
    binary :comparison, :term, %i[GREATER GREATER_EQUAL LESS LESS_EQUAL]
    binary :equality, :comparison, %i[BANG_EQUAL EQUAL_EQUAL]

    def assignment
      # Get any expression.
      expr = equality

      # If the next token is '=', turn this into an assignment.
      if match?(Token::EQUAL)
        equals = previous
        value = assignment

        if expr.is_a?(Expr::Variable)
          # This will get more complex.
          name = expr.name
          return Expr::Assign.new(name, value)
        end

        # Report error but do not throw
        # as parser doesn't need to synchronize here.
        error(equals, "Invalid assignment target.")
      end

      expr
    end

    alias expression assignment

    def statement
      return print_statement if match?(Token::PRINT)
      return block_statement if match?(Token::LEFT_BRACE)

      expression_statement
    end

    def block_statement
      statements = []

      while !current?(Token::RIGHT_BRACE) && !eof?
        statements << declaration
      end

      consume(Token::RIGHT_BRACE, "Expect '}' after block.")
      Stmt::Block.new(statements)
    end

    def print_statement
      value = expression
      consume(Token::SEMICOLON, "Expect ';' after value.")
      Stmt::Print.new(value)
    end

    def expression_statement
      expr = expression
      consume(Token::SEMICOLON, "Expect ';' after expression.")
      Stmt::Expression.new(expr)
    end

    def var_declaration
      name = consume(Token::IDENTIFIER, "Expect variable name.")
      initializer = expression if match?(Token::EQUAL)
      consume(Token::SEMICOLON, "Expect ';' after variable declaration")
      Stmt::Var.new(name, initializer)
    end

    def declaration
      return var_declaration if match?(Token::VAR)

      statement
    rescue Error
      synchronize
    end
  end
end

# frozen_string_literal: true

module Lox
  class Token
    def self.type(name)
      value = name.downcase

      # Define predicate method on Token for each type.
      class_eval <<~CODE
        def #{value}?
          type == #{value.inspect}
        end
      CODE

      const_set(name, value)
    end

    # Single-character tokens.
    type :LEFT_PAREN
    type :RIGHT_PAREN
    type :LEFT_BRACE
    type :RIGHT_BRACE
    type :COMMA
    type :DOT
    type :MINUS
    type :PLUS
    type :SEMICOLON
    type :SLASH
    type :STAR

    # One or two character tokens.
    type :BANG
    type :BANG_EQUAL
    type :EQUAL
    type :EQUAL_EQUAL
    type :GREATER
    type :GREATER_EQUAL
    type :LESS
    type :LESS_EQUAL

    # Literals.
    type :IDENTIFIER
    type :STRING
    type :NUMBER

    # Keywords.
    type :AND
    type :CLASS
    type :ELSE
    type :FALSE
    type :FUN
    type :FOR
    type :IF
    type :NIL
    type :OR
    type :PRINT
    type :RETURN
    type :SUPER
    type :THIS
    type :TRUE
    type :VAR
    type :WHILE

    type :EOF

    attr_reader :lexeme, :line, :literal, :type

    def initialize(type, lexeme, literal, line)
      @type = type
      @lexeme = lexeme
      @literal = literal
      @line = line
    end

    alias to_s inspect
    # def to_s
    #   "#{type} #{lexeme} #{literal}"
    # end
  end
end

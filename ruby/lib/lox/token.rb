# frozen_string_literal: true

module Lox
  class Token
    def self.type(name)
      value = name.downcase.to_sym

      # Define predicate method on Token for each type.
      class_eval <<~CODE
        def #{value}?
          type == #{value.inspect}
        end
      CODE

      const_set(name, value)
    end

    Lox.defs("tokens").each do |token|
      self.type token
    end

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

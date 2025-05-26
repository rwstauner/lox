# frozen_string_literal: true

module Lox
  class Environment
    def initialize
      @values = {}
    end

    def define(name, value)
      @values[name] = value
    end

    # Token
    def get(name)
      @values.fetch(name.lexeme) do
        raise Lox::RuntimeError, "Undefined variable '#{name.lexeme}'."
      end
    end
  end
end

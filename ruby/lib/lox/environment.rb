# frozen_string_literal: true

module Lox
  class Environment
    def initialize
      @values = {}
    end

    def define(name, value)
      @values[name] = value
    end

    def assign(token, value)
      key = token.lexeme
      if @values.key?(key)
        @values[key] = value
        return
      end
      undefined!(key)
    end

    def get(token)
      key = token.lexeme
      @values.fetch(key) do
        undefined!(key)
      end
    end

    def undefined!(name)
      raise Lox::RuntimeError, "Undefined variable '#{name}'."
    end
  end
end

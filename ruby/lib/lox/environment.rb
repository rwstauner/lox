# frozen_string_literal: true

module Lox
  class Environment
    def initialize(enclosing = nil)
      @values = {}
      @enclosing = enclosing
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

      return @enclosing.assign(token) if enclosed?

      undefined!(key)
    end

    def get(token)
      key = token.lexeme
      @values.fetch(key) do
        return @enclosing.get(token) if enclosed?

        undefined!(key)
      end
    end

    def enclosed?
      !@enclosing.nil?
    end

    def undefined!(name)
      raise Lox::RuntimeError, "Undefined variable '#{name}'."
    end
  end
end

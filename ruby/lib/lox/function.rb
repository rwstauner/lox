# frozen_string_literal: true

module Lox
  class Function
    attr_reader :declaration

    def initialize(declaration)
      @declaration = declaration
    end

    def arity
      declaration.params.size
    end

    def call(interpreter, arguments)
      environment = Environment.new(interpreter.environment)

      declaration.params.zip(arguments).each do |param, arg|
        environment.define(param.lexeme, arg)
      end

      interpreter.execute_block(declaration.body.statements, environment)
      nil
    end

    def to_s
      "<fn #{declaration.name.lexeme}>"
    end
  end
end

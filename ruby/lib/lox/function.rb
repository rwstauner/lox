# frozen_string_literal: true

module Lox
  class Function
    attr_reader :declaration

    def initialize(declaration, closure)
      @declaration = declaration
      @closure = closure
    end

    def arity
      declaration.params.size
    end

    def call(interpreter, arguments)
      environment = Environment.new(@closure)

      declaration.params.zip(arguments).each do |param, arg|
        environment.define(param.lexeme, arg)
      end

      catch(:return) do
        interpreter.execute_block(declaration.body.statements, environment)
        nil
      end
    end

    def to_s
      "<fn #{declaration.name.lexeme}>"
    end
    alias inspect to_s

    class Native
      attr_reader :callable, :name

      def initialize(name, &callable)
        @name = name
        @callable = callable
      end

      def arity
        # Interpreter is first arg.
        callable.arity - 1
      end

      def call(interpreter, arguments)
        callable.call(interpreter, *arguments)
      end

      def to_s
        "<native #{name}>"
      end
      alias inspect to_s
    end
  end
end

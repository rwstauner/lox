# frozen_string_literal: true

module Lox
  class Interpreter
    class Error < ::RuntimeError
      attr_reader :token
      def initialize(token, message)
        super(message)
        @token = token
      end
    end

    attr_reader :environment

    GLOBALS = Environment.new.tap do |env|
      env.define_fun(Lox::Function::Native.new("clock") { |_i| Process.clock_gettime(Process::CLOCK_MONOTONIC) })
      env.define_fun(Lox::Function::Native.new("sleep") { |_i, s| sleep(s) })
    end

    def initialize
      @environment = Environment.new(GLOBALS)
    end

    def interpret(statements)
      statements.each do |stmt|
        execute(stmt)
      end
    rescue Error => error
      Lox.runtime_error(error)
    end

    def execute(stmt)
      stmt&.accept(self)
    end

    def execute_block(statements, environment)
      previous_env = @environment
      @environment = environment
      statements.each do |stmt|
        execute(stmt)
      end
    ensure
      @environment = previous_env
    end

    def evaluate(expr)
      expr&.accept(self)
    end

    def visit_literal(expr)
      expr.value
    end

    def visit_grouping(expr)
      evaluate(expr.expression)
    end

    def visit_unary(expr)
      right = evaluate(expr.right)

      case expr.operator.type
      when Token::BANG
        return !bool(right)
      when Token::MINUS
        return -right
      end
    end

    BINARY_NUMERIC_OPERATIONS = {
      Token::MINUS => :-,
      Token::SLASH => :/,
      Token::STAR => :*,
      Token::GREATER => :>,
      Token::GREATER_EQUAL => :>=,
      Token::LESS => :<,
      Token::LESS_EQUAL => :<=,
    }

    def visit_binary(expr)
      left = evaluate(expr.left)
      right = evaluate(expr.right)

      op = expr.operator

      if method = BINARY_NUMERIC_OPERATIONS[op.type]
        ensure_numerics!(op, left, right)
        return left.public_send(method, right)
      end

      case op.type
      when Token::PLUS
        if [left, right].all? { string?(_1) } ||
           [left, right].all? { numeric?(_1) }
          return left + right
        end
        raise Error.new(op, "Operands must be two numbers or two strings.")
      when Token::BANG_EQUAL
        return !vals_equal?(left, right)
      when Token::EQUAL_EQUAL
        return vals_equal?(left, right)
      end
    end

    def visit_logical(expr)
      left = evaluate(expr.left)

      case expr.operator.type
      when Token::OR
        # Short circuit true for OR.
        return left if bool(left)
      when Token::AND
        # Short circuit false for AND.
        return left if !bool(left)
      else
        raise ArgumentError, "Unknown Logical type: #{expr.type}"
      end

      evaluate(expr.right)
    end

    def visit_call(expr)
      callee = evaluate(expr.callee)
      arguments = expr.arguments.map(&method(:evaluate))

      if !callee.respond_to?(:call)
        Lox.error("Can only call functions and classes.", token: expr.paren)
      end

      if arguments.size != callee.arity
        p callee
        Lox.error("Expected #{callee.arity} arguments but got #{arguments.size}.", token: expr.paren)
      end

      callee.call(self, arguments)
    end

    def visit_block_stmt(stmt)
      execute_block(stmt.statements, Environment.new(@environment))
    end

    def visit_expression_stmt(stmt)
      evaluate(stmt.expression)
    end

    def visit_function_stmt(stmt)
      fn = Lox::Function.new(stmt, environment)
      environment.define(stmt.name.lexeme, fn)
      nil
    end

    def visit_if_stmt(stmt)
      if bool(evaluate(stmt.condition))
        execute(stmt.then_branch)
      elsif stmt.else_branch
        execute(stmt.else_branch)
      end
    end

    def visit_while_stmt(stmt)
      while bool(evaluate(stmt.condition))
        execute(stmt.body)
      end
    end

    def visit_print_stmt(stmt)
      value = evaluate(stmt.expression)
      puts stringify(value)
    end

    def visit_return_stmt(stmt)
      value = evaluate(stmt.value) if stmt.value
      throw(:return, value)
    end

    def visit_var_stmt(stmt)
      value = evaluate(stmt.initializer) if !stmt.initializer.nil?
      @environment.define(stmt.name.lexeme, value)
    end

    def visit_variable(expr)
      @environment.get(expr.name)
    end

    def visit_assign(expr)
      value = evaluate(expr.value)
      @environment.assign(expr.name, value)
      value
    end

    def bool(val)
      # Lox follows Ruby’s simple rule: false and nil are falsey, and everything else is truthy.
      val
    end

    def vals_equal?(l, r)
      l == r
    end

    def numeric?(val)
      val.is_a?(Numeric)
    end

    def string?(val)
      val.is_a?(String)
    end

    def ensure_numeric!(operator, operand)
      return if numeric?(operand)
      raise Error.new(operator, "Operand must be a number.")
    end

    def ensure_numerics!(operator, left, right)
      return if numeric?(left) && numeric?(right)
      raise Error.new(operator, "Operands must be numbers.")
    end

    def stringify(val)
      if val.is_a?(Float)
        return val.to_s.delete_suffix(".0")
      end

      val.to_s
    end
  end
end

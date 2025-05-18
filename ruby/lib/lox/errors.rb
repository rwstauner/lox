# frozen_string_literal: true

module Lox
  class Error < RuntimeError
    def initialize(message, location = nil)
      @location = location
      super("#{message}\n\nat:\n#{location}")
    end
  end

  extend self

  def error(msg, token: nil, location: nil)
    had_error!
    at = token.eof? ? "end" : token.lexeme.dump
    printf "%s at %s: %s\n", token.line, at, msg
  end

  def runtime_error(error)
    had_runtime_error!(error)
    puts "#{error.message}\n[line #{error.token.line}]"
  end

  def had_error!(error = true)
    @error = error
  end

  def had_runtime_error!(error)
    @runtime_error = error
  end

  def had_error?
    @error
  end

  def had_runtime_error?
    @runtime_error
  end
end

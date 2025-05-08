# frozen_string_literal: true

module Lox
  class Error < RuntimeError
    def initialize(message, location = nil)
      @location = location
      super("#{message}\n\nat:\n#{location}")
    end
  end

  def self.error(msg, token: nil, location: nil)
    at = token.eof? ? "end" : token.lexeme.dump
    printf "%s at %s: %s\n", token.line, at, msg
  end
end

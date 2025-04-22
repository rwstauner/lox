# frozen_string_literal: true

module Lox
  class Error < RuntimeError
    def initialize(message, location = nil)
      @location = location
      super("#{message}\n\nat:\n#{location}")
    end
  end
end

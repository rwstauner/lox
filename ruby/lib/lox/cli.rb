# frozen_string_literal: true

module Lox
  class CLI
    def self.run!(argv)
      new(argv).run!
    end

    attr_reader :argv, :interpreter

    def initialize(argv)
      @argv = argv
      @interpreter = Interpreter.new
    end

    # TODO: consolidate with errors.rb
    def error(message, location = nil)
      Lox.error(Error.new(message, location).message)
    end

    def run_prompt
      loop do
        print "> "
        line = gets
        break if line.nil?
        run(line)
      rescue => error
        # Show errors but continue looping.
        puts error
        puts error.backtrace
      end
    end

    def run(str)
      expr = Parser.new(Scanner.new(str).scan.tokens).parse

      if expr.is_a?(Parser::Error)
        had_error!(expr)
      else
        # AstPrinter.new(expr)
        interpreter.interpret(expr)
      end
    end

    def run_file(file)
      run(file == "-" ? STDIN.read : File.read(file))
      if Lox.had_error?
        exit(65)
      elsif Lox.had_runtime_error?
        exit(70)
      end
    end

    def run!
      if argv.size > 1
        puts "Usage: rlox [script]"
        exit(64)
      elsif argv.size == 1
        run_file(argv[0])
      else
        run_prompt
      end
    end
  end
end

# frozen_string_literal: true

module Lox
  class CLI
    def self.run!(argv)
      new(argv).run!
    end

    attr_reader :argv

    def initialize(argv)
      @argv = argv
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
      end
    end

    def run(str)
      expr = Parser.new(Scanner.new(str).scan.tokens).parse

      exit(65) if expr.is_a?(Parser::Error)

      AstPrinter.new(expr)
    end

    def run_file(file)
      run(file == "-" ? STDIN.read : File.read(file))
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

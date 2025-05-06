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

    def error(message, location = nil)
      puts Error.new(message, location).message
    end

    def run_prompt
      loop do
        puts "> "
        line = gets
        break if line.nil?
        result = run(line)
        if result.error?
          exit(65)
        end
      end
    end

    def run(str)
      Scanner.new(str).scan.tokens.each do |token|
        p token
      end
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

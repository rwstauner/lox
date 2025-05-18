# frozen_string_literal: true

module Lox
  class Scanner
    attr_reader :errors, :source, :tokens

    def initialize(source)
      @source = source
      @tokens = []
      @start = 0
      @current = 0
      @line = 1
      @errors = []
    end

    def scan
      while !eof?
        @start = @current
        scan_token
      end
      @tokens << Token.new(Token::EOF, "", nil, @line)
      self
    end

    def add_token(type, literal = nil)
      @tokens << Token.new(type, substring, literal, @line)
    end

    private

    def substring(a = @start, b = @current)
      @source[a...b]
    end

    def advance
      @source[@current].tap do
        @current += 1
      end
    end

    STATIC_TOKENS = {
      '(' => Token::LEFT_PAREN,
      ')' => Token::RIGHT_PAREN,
      '{' => Token::LEFT_BRACE,
      '}' => Token::RIGHT_BRACE,
      ',' => Token::COMMA,
      '.' => Token::DOT,
      '-' => Token::MINUS,
      '+' => Token::PLUS,
      ';' => Token::SEMICOLON,
      '*' => Token::STAR,
      '/' => Token::SLASH,
      '!'  => Token::BANG,
      '!=' => Token::BANG_EQUAL,
      '='  => Token::EQUAL,
      '==' => Token::EQUAL_EQUAL,
      '>'  => Token::GREATER,
      '>=' => Token::GREATER_EQUAL,
      '<'  => Token::LESS,
      '<=' => Token::LESS_EQUAL,
    }

    def scan_token
      c = advance

      # A comment consumes the rest of the line, like this one.
      return scan_comment if c == '/' && match?('/')

      # Consume the "=" if it follows.
      if %w[! = < >].include?(c)
        c += '=' if match?('=')
      end

      if type = STATIC_TOKENS[c]
        return add_token(type)
      end

      case c
      when ' ', "\r", "\t"
        # ignore
      when "\n"
        @line += 1
      when '"'
        scan_string
      else
        if digit?(c)
          scan_number
        elsif start_identifier?(c)
          scan_identifier
        else
          error("Unexpected character")
        end
      end
    end

    def match?(expected)
      return false if eof? || peek != expected

      @current += 1
      true
    end

    def eof?(offset = 0)
      (@current + offset) >= @source.size
    end

    def peek(offset = 0)
      return "\0" if eof?(offset)

      @source[@current + offset]
    end

    def error(message, cls = Lox::Error)
      @errors << cls.new(message, @line)
    end

    DIGITS = '0'..'9'
    def digit?(c)
      DIGITS.include?(c)
    end

    START_IDENTIFIER = ('a' .. 'z').flat_map { [_1, _1.upcase] } + ['_']
    def start_identifier?(c)
      START_IDENTIFIER.include?(c)
    end

    def identifier?(c)
      # Identifiers can contain digits but don't start with them.
      start_identifier?(c) || digit?(c)
    end

    def scan_comment
      while peek != "\n" && !eof?
        advance
      end
      # current "token" is a comment
    end

    def scan_number
      advance while digit?(peek)

      if peek == '.' && digit?(peek(1))
        advance
        advance while digit?(peek)
      end

      add_token(Token::NUMBER, substring.to_f)
    end

    def scan_string
      while peek != '"' && !eof?
        @line += 1 if peek == "\n"
        advance
      end

      if eof?
        return error("Unterminated string.")
      end

      # Consume the closing '"'.
      advance

      add_token(Token::STRING, substring(@start + 1, @current - 1))
    end

    RESERVED_WORDS = {
      "and"    => Token::AND,
      "class"  => Token::CLASS,
      "else"   => Token::ELSE,
      "false"  => Token::FALSE,
      "for"    => Token::FOR,
      "fun"    => Token::FUN,
      "if"     => Token::IF,
      "nil"    => Token::NIL,
      "or"     => Token::OR,
      "print"  => Token::PRINT,
      "return" => Token::RETURN,
      "super"  => Token::SUPER,
      "this"   => Token::THIS,
      "true"   => Token::TRUE,
      "var"    => Token::VAR,
      "while"  => Token::WHILE,
    }
    def scan_identifier
      advance while identifier?(peek)

      name = substring
      type = RESERVED_WORDS.fetch(name) { Token::IDENTIFIER }
      add_token(type)
    end
  end
end

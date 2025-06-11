# frozen_string_literal: true

module Lox
  class Stmt
    def self.type(name, attrs)
      class_eval <<~CODE
        class #{name} < Stmt
          attr_reader #{attrs.map(&:inspect).join(", ")}

          def initialize(#{attrs.join(", ")})
            #{attrs.map { |att| "@#{att} = #{att}" }.join("\n") }
          end

          def accept(visitor)
            visitor.visit_#{name.downcase}_stmt(self)
          end
        end
      CODE
    end

    Lox.defs("stmt").each do |type, args|
      # Discard the type.
      self.type type, args.map { |_type, name| name }
    end
  end
end

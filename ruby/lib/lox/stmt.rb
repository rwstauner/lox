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

    type :Expression, %i[expression]
    type :Print, %i[expression]
    type :Var, %i[name initializer]
  end
end

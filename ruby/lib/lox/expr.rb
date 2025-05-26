# frozen_string_literal: true

module Lox
  class Expr
    def self.type(name, attrs)
      class_eval <<~CODE
        class #{name} < Expr
          attr_reader #{attrs.map(&:inspect).join(", ")}

          def initialize(#{attrs.join(", ")})
            #{attrs.map { |att| "@#{att} = #{att}" }.join("\n") }
          end

          def accept(visitor)
            visitor.visit_#{name.downcase}(self)
          end
        end
      CODE
    end

    type :Binary, %i[left operator right]
    type :Grouping, %i[expression]
    type :Literal, %i[value]
    type :Unary, %i[operator right]
    type :Variable, %i[name]
  end
end

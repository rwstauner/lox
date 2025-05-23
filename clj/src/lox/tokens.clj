(ns lox.tokens
  (:require [clojure.string :as str]))

(def types (atom #{}))

(defn token
  [type lexeme literal line]
  {:type type
   :lexeme lexeme
   :literal literal
   :line line})

(defmacro def-types [& names]
  (->> names
       (map
         (fn [n]
           `(do
              (swap! types conj ~n)
              (def ~(symbol n) ~n)
              (defn ~(symbol (str (name n) "?")) [x#] (= x# ~n)))))
       (cons 'do)))

(def-types
  ; single-character tokens.
  :left-paren
  :right-paren
  :left-brace
  :right-brace
  :comma
  :dot
  :minus
  :plus
  :semicolon
  :slash
  :star

  ; one or two character tokens.
  :bang
  :bang-equal
  :equal
  :equal-equal
  :greater
  :greater-equal
  :less
  :less-equal

  ; literals.
  :identifier
  :string
  :number

  ; keywords.
  :and
  :class
  :else
  :false
  :fun
  :for
  :if
  :nil
  :or
  :print
  :return
  :super
  :this
  :true
  :var
  :while

  :eof
  )

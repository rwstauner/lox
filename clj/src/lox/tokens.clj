(ns lox.tokens
  (:require
    [clojure.string :as str]
    [lox.helpers :as h]))

; (def -types (atom #{}))

(defn token
  [type lexeme literal line]
  {:type type
   :lexeme lexeme
   :literal literal
   :line line})

(defn- make-token
  [tok]
  ; If we use a prefix we don't have to use ns-unmap or refer-clojure/exclude
  ; for the large number of standard functions we'll be overwriting.
  ; It could also lead to confusion and bugs if this ns ever gets any more logic in the
  ; future if so many standard functions don't work normally.
  (let [prefix "-"
        attr (str prefix tok)
        pred (str attr "?")]
    ; Add to types set.
    ; (swap! -types conj tok)
    ; (def foo "foo")
    ; (ns-unmap *ns* (symbol attr))
    (intern *ns* (symbol attr) tok)
    ; (defn foo? [x] (= x "foo"))
    ; (ns-unmap *ns* (symbol pred))
    (intern *ns* (symbol pred) (partial = tok))))

(doseq [tok (map h/name->clj (h/defs "tokens"))]
  (make-token tok))

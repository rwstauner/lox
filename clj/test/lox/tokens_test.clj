(ns lox.tokens-test
  (:require
    [clojure.test :refer [deftest is testing]]
    [lox.tokens :as t]))

(deftest tokens-created-from-defs
  (is (= "left-brace" t/-left-brace))
  (is (t/-equal? t/-equal))
  (is (not (t/-slash? t/-star))))

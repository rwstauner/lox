(ns lox.helpers
  (:require [clojure.java.io :as io]
            [clojure.string :as str]))

(defn name->clj [n]
  (-> n
      str/lower-case
      (str/replace #"_" "-")))

(defn defs [base]
  (-> *file*
      io/file
      .toPath
      .getParent
      (.resolve (str "../../../defs/" base ".def"))
      (.normalize)
      (.toString)
      (slurp)
      (str/split #"\n")
      (->>
        (remove #(re-matches #"^\s*(#.*)?$" %)))))

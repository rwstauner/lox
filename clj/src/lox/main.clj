(ns lox.main
  (:require [lox.scanner :as s]))

(defn -main [& args]
  (prn args)
  (s/scan (slurp (first args))))

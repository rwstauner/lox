(ns lox.scanner
  (:require [clojure.string :as str]
            [lox.tokens :as t]))

(def start-ident?
  (set (str/split "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" #"")))

(def digit? (set (str/split "0123456789" #"")))

(defn ident?
  [s]
  (or
    (start-ident? s)
    (digit? s)))

; TODO: :start/:current
(defn substring [ctx start end]
  (subs (:source ctx) start end))

(def context
  {:tokens []
   :start 0
   :current 0
   :line 1
   :errors []})

(defn advance [ctx]
  (let [{:keys [source start current]} ctx
        c (subs source current 1)]
    (-> ctx
      (assoc :c c)
      (update current inc))))

(defn eof?
 ([ctx]
  (eof? ctx 0))
 ([{:keys [current size]} offset]
  (>= (+ current offset)
      size)))

(defn scan-comment [ctx]
  (loop [ctx ctx]
    ; TODO: find in str instead of loop?
    ; TODO: not eof?
    (if (not= "\n" (peek ctx))
      (recur (advance ctx))
      ctx)))

(defn scan-token [ctx]
  (let [{:keys [c] :as ctx} (advance ctx)]
    (cond
      (and (= "/" c) (= "/" (peek ctx)))
      (scan-comment (advance ctx))

      ; !=<> static tokens ()

      ; TODO: exit loop
      )
    ))

(defn scan-loop [context]
  (loop [ctx context]
    (if (eof? ctx)
      (update ctx :tokens conj t/eof)
      (recur (scan-token
               (assoc ctx :start (:current ctx)))))))

(defn scan [source]
  (-> context
      (assoc :source source)
      (assoc :size (count source))
      scan-loop))

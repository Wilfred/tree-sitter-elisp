(defvar my-var nil "Docstring.")
;;      ^ variable
;;             ^ constant.builtin
;;                  ^ string.documentation

(defconst my-const 1 "Docstring.")
;;        ^ variable
;;                 ^ number
;;                    ^ string.documentation

;; The second argument is the initial value, not a docstring.
(defvar other-var "Not a docstring.")
;;      ^ variable
;;                 ^ string

(let ((x 1)
;;     ^ variable
      (y 2))
;;     ^ variable
  (+ x y))

(let* (a (b 3))
;;     ^ variable
;;        ^ variable
  (list a b))

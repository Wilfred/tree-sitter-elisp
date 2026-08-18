(defun foo (x)
;;^ keyword
  ;;   ^ function
  ;;        ^ variable.parameter
  "stuff"
;; ^ string.documentation
  x)

(defun bar (x &optional y &rest zs)
  ;;          ^ keyword
  ;;                    ^ variable.parameter
  ;;                      ^ keyword
  ;;                            ^ variable.parameter
  (list x y zs))

(defmacro baz (a &rest body)
  ;;^ keyword
  ;;      ^ function
  ;;           ^ variable.parameter
  ;;             ^ keyword
  ;;                   ^ variable.parameter
  "Docstring."
;; ^ string.documentation
  body)

(mapcar #'foo xs)
;;      ^ operator
;;        ^ function

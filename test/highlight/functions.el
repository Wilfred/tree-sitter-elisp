(defun foo (x)
  ;; ^ keyword
  ;;   ^ function
  ;;        ^ variable.parameter
  "stuff"
  ;; ^ string
  x)

(mapcar #'foo xs)
;;      ^ operator
;;        ^ function

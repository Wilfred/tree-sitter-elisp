(setq plist '(:foo 1 :bar-baz 2))
;;              ^ constant
;;                    ^ constant

(defvar my-var :value)
;;              ^ constant

(list :a nil t x)
;;    ^ constant
;;       ^ constant.builtin
;;           ^ constant.builtin

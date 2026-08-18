(list 1 2.5 ?a "text")
;;    ^ number
;;      ^ number
;;          ^ number
;;              ^ string

(list ?\C-x ?\N{LATIN SMALL LETTER A})
;;    ^ number
;;          ^ number

(vconcat [1 2] "x")
;;       ^ punctuation.bracket
;;           ^ punctuation.bracket

(list #[1 2])
;;    ^ punctuation.bracket

(list #("text" 0 4 (face bold)))
;;    ^ punctuation.bracket

;; #$ is the name of the file being loaded.
(list #$)
;;    ^ constant.builtin

(list `(a ,b ,@c) 'd)
;;    ^ operator
;;        ^ operator
;;           ^ operator
;;                ^ operator

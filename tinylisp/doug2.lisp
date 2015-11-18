(defun balls (a b c) (
	(+ a b c)
	))

(let endl "\n")

(print "hello world")
(print "nil test:" nil ())
(print "call test:" (balls 1 2 3))
(print "show list:" (1 2 3))
(print)

(print "comparison tests:")
(print "maths:"
	endl
	(= 1 1)
	(= 1 2)
	(= 1 1 2)
	(= (+ 1 2) 3))
(print "lists and nil:"
	endl
	(= (3 1 2) 3)
	(= nil nil)
	(= nil (1 2 3))
	; (= nil not_defined)  ; runtime error
	(= nil balls)
	endl)

(print "len operator:"
	(len balls)
	endl
	"len with caps:"
	(LEN BALLS))

(print "compare:"
	endl "greater:"
	(> 2 1)
	(> 1 2)
	(> 3 1 2)
	(> 3 1 2 3)
	endl "less:"
	(< 1 2)
	(< 2 1 0)
	endl "greater-equal"
	(>= 2 2)
	(>= 2 2 3)
	(>= 2 1)
	endl "less-equal"
	(<= 2 2)
	(<= 2 3)
	(<= 2 3 1)
	endl)

(print "while test:")
(let i 10)
(while (>= 1 i) (
	(print " i =" i)
	(let i (- i 1))
	))
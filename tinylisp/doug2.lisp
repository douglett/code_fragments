(define balls (a b c)
	(+ a b c))

(define endl "\n")

(print "call test:" (balls 1 2 3) endl)

(print "list" (1 2 3) endl)

(print "hello world")
(print nil ())
(print)

(print "comparison tests:")
(print "maths:"
	(= 1 1)
	(= 1 2)
	(= 1 1 2)
	(= (+ 1 2) 3))
(print "lists and nil:"
	(= (3 1 2) 3)
	(= nil nil)
	(= nil (1 2 3))
	(= nil not_defined)
	(= nil balls)
	endl)

(print "len operator:"
	(len balls))
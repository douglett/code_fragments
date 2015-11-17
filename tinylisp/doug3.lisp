(defun a () (
	(print "a ran")
	(print "print another line")
	))

(a 1 2)

(let b "hello")
(print b)

(let i 1)
(print (> 2 i))

(while (<= 10 i) (
	(print i)
	(let i (+ i 1))
	))

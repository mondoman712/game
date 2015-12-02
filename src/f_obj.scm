(define (parse-vector str)
  (map string->number (cdr (string-split str #\space))))

(define (parse-face str)
  (map (lambda (x)
	 (map (lambda (y)
		(if (string-null? y) -1
		    (- (string->number y) 1))) x))
       (map (lambda (s) (string-split s #\/))
	    (cdr (string-split str #\space)))))

(define* (read-obj-lines file #:optional (vlst '()) (vtlst '()) (vnlst '()) (flst '()))
  (let ((line (read-line file)))
    (if (not (eof-object? line))
	(cond ((string-contains line "v " 0 2)
	       (read-obj-lines file (append vlst (list (parse-vector line))) vtlst vnlst flst))
	      ((string-contains line "vt " 0 3)
	       (read-obj-lines file vlst (append vtlst (list (parse-vector line))) vnlst flst))
	      ((string-contains line "vn " 0 3)
	       (read-obj-lines file vlst vtlst (append vnlst (list (parse-vector line))) flst))
	      ((string-contains line "f " 0 2)
	       (read-obj-lines file vlst vtlst vnlst (append flst (parse-face line))))
	      (else (read-obj-lines file vlst vtlst vnlst flst)))
	(list vlst vtlst vnlst flst))))

(define (parse-obj lst)
  (append-map (lambda (l)
		(append (list-ref (list-ref lst 0) (list-ref l 0))
			(list-ref (list-ref lst 1) (list-ref l 1))
			(list-ref (list-ref lst 2) (list-ref l 2))))
	      (list-ref lst 3)))

(define (parse-obj lst)
  

(define (load-obj filename)
  (let ((fp (open-file filename "r")))
    (parse-obj (read-obj-lines fp))))

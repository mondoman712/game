(define (string->numlist str)
  (map string->number (string-split str #\space)))

(define (read-obj-lines file lst)
  (let ((line (read-line file)))
    (if (eof-object? line)
	(if (string-contains line "v " 0 2)
	    (begin
	      (display line)
	      (newline)
	      (read-obj-lines file (append lst (string->numlist (substring line 2)))))
	    (read-obj-lines file lst))
	lst)))

(define (load-obj filename)
  (let ((fp  (open-file filename "r")))
    (read-obj-lines fp '())))

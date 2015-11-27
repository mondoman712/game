(define (string->numlist str)
  (map string->number (string-split str #\space)))

(define (read-obj-lines file)
  (let ((line (read-line file)))
    (unless (eof-object? line)
      (display line)
      (newline)
      (read-obj-lines file))))

(define (load-obj filename)
  (let ((fp  (open-file filename "r")))
    (read-obj-lines fp)))

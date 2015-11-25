(define (string->numlist str)
  (map string->number (string-split str #\space)))

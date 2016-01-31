(define (parse-vector str)
  (map string->number (cdr (string-split str #\space))))

(define (parse-face str)
  (map (lambda (x)
	 (map (lambda (y)
		(if (string-null? y) -1
		    (- (string->number y) 1))) x))
       (map (lambda (s) (string-split s #\/))
	    (cdr (string-split str #\space)))))

(define* (read-obj-lines file #:optional (vlst '()) (vtlst '()) (vnlst '()) (flst '()) mtl)
  "Reads lines from obj file and deposits different vertex types into different lists"
  (let ((line (read-line file)))
    (if (not (eof-object? line))
	(cond ((string-contains line "v " 0 2)
	       (read-obj-lines file (append vlst (list (parse-vector line))) vtlst vnlst flst mtl))
	      ((string-contains line "vt " 0 3)
	       (read-obj-lines file vlst (append vtlst (list (parse-vector line))) vnlst flst mtl))
	      ((string-contains line "vn " 0 3)
	       (read-obj-lines file vlst vtlst (append vnlst (list (parse-vector line))) flst mtl))
	      ((string-contains line "f " 0 2)
	       (read-obj-lines file vlst vtlst vnlst (append flst (parse-face line)) mtl))
	      ((string-contains line "mtllib ")
	       (read-obj-lines file vlst vtlst vnlst flst (list-ref (string-split line #\space) 1)))
	      (else (read-obj-lines file vlst vtlst vnlst flst mtl)))
	(list vlst vtlst vnlst flst mtl))))

(define (parse-obj lst)
  "Converts lists from read-obj-lines into 1 list of vertices"
  (let ((get-vertex
	 (lambda (l x)
	   (if (= (list-ref l x) -1)
	       (list 0.0 0.0)
	       (list-ref (list-ref lst x) (list-ref l x))))))
    (list 
     (append-map (lambda (l)
		   (append (get-vertex l 0)
			   (get-vertex l 1)
			   (get-vertex l 2)))
		 (list-ref lst 3))
     (list-ref lst 4))))

(define (load-obj filename)
  (let ((fp (open-file filename "r")))
    (parse-obj (read-obj-lines fp))))

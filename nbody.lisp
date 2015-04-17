;;;; N BODY SIMULATOR

(ql:quickload "lispbuilder-sdl")

;; define some stuff
(defclass point ()
  ((x :type number
      :accessor x
      :initarg :x
      :initform 0)
   (y :type number
      :accessor y
      :initarg :y
      :initform 0)))


(defclass body ()
  ((pos :type point
	:accessor pos
	:initarg :pos
	:initform 0)
   (vel :type point
	:accessor vel
	:initarg :vel
	:initform 0)
   (mass :type number
	 :accessor mass
	 :initarg :mass
	 :initform 0)))

(defparameter *screen-size* (make-instance 'point :x 1000 :y 700))
(defparameter *G* 6.67e-11)
(defparameter *quit* 'nil)
(defparameter *scale* 1e11)

;; functions for file io
(defun body-to-list (body)
  "Converts a body into a list of its attributes"
  (list (x (pos body))         
	(y (pos body))         
	(x (vel body))         
	(y (vel body))         
	(mass body)))         

(defun bodies-to-list (bodies)
  "Calls body-to-list on a list of bodies"
  (mapcar #'body-to-list bodies))

(define-condition invalid-filename (error)
  ((filename :initarg :filename :accessor filename)))

(defun check-filename (filename)
  (if
   (zerop
    (count-if #'characterp
	      (mapcar #'(lambda (c) (find c filename :test #'equalp))
		      (cond ((string= (software-type) "Linux")
			     '(#\/ #\NULL #\; #\: #\|))
			    ((string= (software-type) "Windows")
			     '(#\< #\> #\: #\/ #\\ #\| #\? #\*))
			    (t 'nil)))))
   filename
   (error 'invalid-filename)))

(defun save-list (lst filename)
  "Saves the list given to a file with the name given"
  (with-open-file (out filename
		       :direction :output
		       :if-exists :supersede)
    (with-standard-io-syntax
      (print lst out))))

(defun save-bodies (bodies filename)
  "Calls save-list and body-to-list"
  (handler-case
      (save-list (bodies-to-list bodies)
		 (check-filename filename))
    (invalid-filename ()
      (princ "Please enter a valid filename"))))

(defun read-list (filename)
  "Reads from a file with the name given"
  (with-open-file (in filename)
    (with-standard-io-syntax
      (read in))))

(defun read-bodies (filename)
    "Reads from a file and converts the lists in the file
   into instances of the body class"
    (loop for bod in (read-list filename)
       collecting (make-instance
		   'body
		   :pos (make-instance
			 'point
			 :x (nth 0 bod)
			 :y (nth 1 bod))
		   :vel (make-instance
			 'point
			 :x (nth 2 bod)
			 :y (nth 3 bod))
		   :mass (nth 4 bod))))


;; functions for maths and shit
(defun pos2pos (pos centre)
  "Converts position relative to centre in km to sdl coordinates"
  ; Defines a local function to convert a coordinate relative to sun in km
  ;  to SDL coodinates, which are in pixels and relative to the top right corner
  (flet ((new-coord (fn xy)
           (funcall fn (/ (slot-value *screen-size* xy) 2) 
                       (slot-value pos xy))))
    ; Calls new-coord on the x and y and creates an SDL point out of them
    (sdl:point :x (- (new-coord #'+ 'x) (x centre))
               :y (+ (new-coord #'- 'y) (y centre)))))

(defmacro sets (fn a b)
  "like setf, but applies fn to the values"
  `(setf ,a (funcall ,fn ,a ,b)))

(defun ang (a b)
  "Calculates the bearing of a line between a and b"
  (atan (- (y a) (y b))
        (- (x a) (x b))))

(defun dist (a b)
  "Calculates the distance between 2 points"
  (sqrt (+ (expt (abs (- (x a) (x b))) 2)
	      (expt (abs (- (y a) (y b))) 2))))

(defun split-force (mag ang)
  "Splits the force into its x and y components, returning them as a point"
  (make-instance 'point :x (* mag (cos ang))
                        :y (* mag (sin ang))))

(defun point+ (&rest points)
  "adds the x and y values of the points given"
  (make-instance 'point
		 :x (apply #'+ (mapcar #'x points))
		 :y (apply #'+ (mapcar #'y points))))

(defun point/ (pt num)
  (make-instance 'point
		 :x (/ (x pt) num) 
		 :y (/ (y pt) num)))

(defun find-acel (body bodies)
  (apply #'point+
	 (mapcar
	  #'(lambda (x)
	      (handler-case 
		  (split-force (calc-g (mass x) (dist (pos body) (pos x)))
			       (ang (pos x) (pos body)))
	      (division-by-zero ()
				(return-from find-acel
				      (handle-collision x body bodies)))))
	  (remove body bodies))))

(defun update-pos (body)
  "Updates the position of a body using its velocities"
  (sets #'+ (x (pos body)) (x (vel body)))
  (sets #'+ (y (pos body)) (y (vel body))))

(defun calc-g (M r)
  "Calculates the acceleration due to gravity"
  (/ (* *G* M) (* r r)))

(defun point= (a b)
  (and (= (x a) (x b)) (= (y a) (y b))))

(define-condition repeated-bodies (error)
  ((repeat :initarg :repeat :accessor repeat)))

(defun collidep (bod1 bod2)
  "Checks if the two bodies are colliding"
  (if (point= (pos bod1) (pos bod2))
      (error 'repeated-bodies)
      (if (< (dist (pos bod1) (pos bod2))
	     (+ (+ 1 (ceiling (/ (mass bod1) *scale*)))
		(+ 1 (ceiling (/ (mass bod2) *scale*)))))
	  't)))

(defun sum-masses (&rest bods)
  "Returns the sum of the masses of the given bodies"
  (apply #'+ (mapcar #'mass bods)))

(defun total-momentum (&rest bods)
  (flet ((mom (bods xy)
	   (apply #'+
		  (mapcar #'(lambda (x) (* (mass x) (slot-value (vel x) xy)))
			  bods))))
    (make-instance 'point :x (mom bods 'x) :y (mom bods 'y))))

(defun handle-collision (bod1 bod2 bodies)
  (append (remove bod1 (remove bod2 bodies))
	(list (make-instance 'body
		       :pos (centre-of-mass (list bod1 bod2))
		       :vel (point/ (total-momentum bod1 bod2)
				    (sum-masses bod1 bod2))
		       :mass (sum-masses bod1 bod2)))))

(defun centre-of-mass (bodies)
  "Returns the centre of mass of the list of bodies given"
  (flet ((centre (bods xy)
	   (/ (apply #'+
		     (mapcar #'(lambda (x) (* (mass x) (slot-value (pos x) xy)))
			     bods))
	      (apply #'+ (mapcar #'mass bods)))))
    (make-instance 'point
		   :x (centre bodies 'x)
		   :y (centre bodies 'y))))

(defun find-acels (bodies)
  (mapcar #'(lambda (bod)
	      (sets #'point+ (slot-value bod 'vel)
		    (find-acel bod bodies)))
	  bodies))

(defun draw-bodies (bodies)
  (let ((centre (centre-of-mass *bodies*)))
    (mapc #'(lambda (x)
	      (handler-case
		  (draw-body x centre)
		(repeated-bodies ()
		  (setq bodies (remove x bodies)))))
	  bodies)))

;; functions for the sdl shhiz
(defun draw-body (body centre)
  "Draws a body to the screen"
  (sdl:draw-filled-circle
   (pos2pos (pos body) centre)
   (+ 1 (ceiling (/ (expt (mass body) 1/3) (/ *scale* 2)))) :color sdl:*white*))

(defun centre-dots (bodies)
  (progn
    (sdl:draw-pixel (pos2pos
		     (make-instance 'point :x 0 :y 0)
		     (centre-of-mass bodies))
		    :color sdl:*red*)
    (sdl:draw-pixel (pos2pos
		     (make-instance 'point :x 0 :y 0)
		     (make-instance 'point :x 0 :y 0))
		    :color sdl:*red*)))

(defun sdl-init ()
  (sdl:window (x *screen-size*)
	      (y *screen-size*)
              :title-caption "nbody")
  (setf (sdl:frame-rate) 60))

(defun sdl-main-loop (bodies)
  (sdl:with-events ()
    (:quit-event () t)
    (:key-down-event
     (:key key)
     (when (sdl:key= key :sdl-key-q)
       (sdl:push-quit-event)
       (setq *bodies* bodies)))
    
    (:idle ()
	   (when *quit* 
	     (sdl:quit-sdl))
	   (sdl:clear-display sdl:*black*)

	   (find-acels bodies)
	   (mapc #'update-pos bodies)

	   (centre-dots bodies)
	   (draw-bodies bodies)


	   (let ((tmpbods bodies))
	     (dolist (i bodies)
	       (progn (setq tmpbods (remove i tmpbods))
		      (dolist (j tmpbods)
			(if (collidep i j)
			    (setq bodies (handle-collision i j bodies)))))))

	   (sdl:update-display))))

(defun main ()
  (sdl:with-init ()
    (sdl-init)
    (sdl-main-loop *bodies*))) 

(defun genbods (n)
  (loop for i from 1 to n
     collecting (make-instance 'body
			       :pos (make-instance 'point
						   :x (- 300 (random 600))
						   :y (- 300 (random 600)))
			       :vel (make-instance 'point
						   :x 0 ;(- 2 (random 4))
						   :y 0 ;(- 2 (random 4))
						   )
			       :mass *scale*)))

(defparameter *bodies* (read-bodies "list1.txt"))

(defparameter *bodies* (list (make-instance 'body
					    :pos (make-instance 'point :x 0 :y 0)
					    :vel (make-instance 'point :x 0 :y 0)
					    :mass *scale*)
			     (make-instance 'body
					    :pos (make-instance 'point :x 0 :y 0)
					    :vel (make-instance 'point :x 0 :y 0)
					    :mass *scale*)))
					    
					    

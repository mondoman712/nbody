(ql:quickload "lispbuilder-sdl")

(defparameter *G* 6.67e-11)
(defparameter *quit* 'nil)
(defparameter *scale* 1e12)

(defclass point ()
  ((x :type number
      :accessor x
      :initarg :x
      :initform 0)
   (y :type number
      :accessor y
      :initarg :y
      :initform 0)))

(defparameter *screen-size* (make-instance 'point :x 1600 :y 1000))

(defclass body ()
  ((pos :type point
	:accessor pos
	:initarg :pos
	:initform 0)
   (mass :type number
	 :accessor mass
	 :initarg :mass
	 :initform 0)
   (vel :type point
	:accessor vel
	:initarg :vel
	:initform 0)))

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

(defun find-acel (body bodies)
  (apply #'point+
	 (mapcar
	  #'(lambda (x)
	      (split-force (calc-g (mass x) (dist (pos body) (pos x)))
			   (ang (pos x) (pos body))))
	  (remove body bodies))))

(defun update-pos (body)
  "Updates the position of a body using its velocities"
  (sets #'+ (x (pos body)) (x (vel body)))
  (sets #'+ (y (pos body)) (y (vel body))))

(defun calc-g (M r)
  "Calculates the acceleration due to gravity"
  (/ (* *G* M) (* r r)))

(defun draw-body (body centre)
  "Draws a body to the screen"
  (sdl:draw-filled-circle
   (pos2pos (pos body) centre)
   (+ 1 (ceiling (/ (mass body) *scale*))) :color sdl:*white*))

(defun collidep (bod1 bod2)
  "Checks if the two bodies are colliding"
  (if (< (dist (pos bod1) (pos bod2))
	 (+ (ceiling (/ (mass bod1) *scale*))
	    (ceiling (/ (mass bod2) *scale*))))
      't))

(defun sum-masses (&rest bods)
  "Returns the sum of the masses of the given bodies"
  (apply #'+ (mapcar #'mass bods)))

(defun total-momentum (&rest bods)
  (flet ((mom (bods xy)
	   (apply #'+
		  (mapcar #'(lambda (x) (* (mass x) (slot-value (pos x) xy)))
			  bods))))
    (make-instance 'point :x (mom bods 'x) :y (mom bods 'y))))

(defun handle-collision (bod1 bod2 bodies)
  (list (remove bod1 (remove bod2 bodies))
	(make-instance 'body
		       :pos (centre-of-mass bod1 bod2)
		       :vel (/ (total-momentum bod1 bod2) (sum-masses bod1 bod2))
		       :mass (sum-masses bod1 bod2))))

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

(defun sdl-init ()
  (sdl:window (x *screen-size*)
	      (y *screen-size*)
              :title-caption "nbody")
  (setf (sdl:frame-rate) 60))

(defun sdl-main-loop ()
  (sdl:with-events ()
    (:quit-event () t)
    (:key-down-event
     (:key key)
     (when (sdl:key= key :sdl-key-q)
       (sdl:push-quit-event)))
    
    (:idle ()
	   (when *quit* 
	     (sdl:quit-sdl))
	   (sdl:clear-display sdl:*black*)

	   (mapcar #'(lambda (bod)
		       (sets #'point+ (slot-value bod 'vel)
			     (find-acel bod *bodies*)))
		   *bodies*)

	   (mapc #'update-pos *bodies*)
	   (let ((centre (centre-of-mass *bodies*)))
	     (mapc #'(lambda (x) (draw-body x centre)) *bodies*))

	   (sdl:draw-pixel (pos2pos
			    (make-instance 'point :x 0 :y 0)
			    (centre-of-mass *bodies*))
			   :color sdl:*red*)

	   (sdl:draw-pixel (pos2pos
			    (make-instance 'point :x 0 :y 0)
			    (make-instance 'point :x 0 :y 0))
			   :color sdl:*red*)

	   (loop for i in *bodies*
	      do (loop for j in (remove i *bodies*)
		    do (if (collidep i j)
			   (setq *bodies* (handle-collision i j *bodies*)))))

	   (sdl:update-display))))

(defun main ()
  (sdl:with-init ()
    (sdl-init)
    (sdl-main-loop))) 

(defparameter *bodies* (list
			(make-instance 'body
				       :pos (make-instance 'point :x -200 :y 0)
				       :vel (make-instance 'point :x 0 :y 2)
				       :mass 1e12)
			(make-instance 'body 
				       :pos (make-instance 'point :x 20 :y 0)
				       :vel (make-instance 'point :x 0 :y 2)
				       :mass 5e12)
			(make-instance 'body 
				       :pos (make-instance 'point :x -20 :y 0)
				       :vel (make-instance 'point :x 0 :y -2)
				       :mass 5e12)
			(make-instance 'body
				       :pos (make-instance 'point :x 0 :y -200)
				       :vel (make-instance 'point :x -2 :y 0)
				       :mass 1e12)
			(make-instance 'body
				       :pos (make-instance 'point :x 200 :y 0)
				       :vel (make-instance 'point :x 0 :y -2)
				       :mass 1e12)
			(make-instance 'body
				       :pos (make-instance 'point :x 0 :y 200)
				       :vel (make-instance 'point :x 2 :y 0)
				       :mass 1e12)
))

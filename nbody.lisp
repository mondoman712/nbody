(ql:quickload "lispbuilder-sdl")

(defparameter *G* 6.67e-11)
(defparameter *quit* 'nil)

(defclass point ()
  ((x :type number
      :accessor x
      :initarg :x
      :initform 0)
   (y :type number
      :accessor y
      :initarg :y
      :initform 0)))

(defparameter *screen-size* (make-instance 'point :x 640 :y 480))

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
	:initform 0)
   (acel :type point
	 :accessor acel
	 :initarg :acel
	 :initform 0)))

(defun pos2pos (pos)
  "Converts position relative to centre in km to sdl coordinates"
  ; Defines a local function to convert a coordinate relative to sun in km
  ;  to SDL coodinates, which are in pixels and relative to the top right corner
  (flet ((new-coord (fn xy)
           (funcall fn (/ (slot-value *screen-size* xy) 2) 
                       (/ (slot-value pos xy) 468750))))
    ; Calls new-coord on the x and y and creates an SDL point out of them
    (sdl:point :x (new-coord #'+ 'x)
               :y (new-coord #'- 'y))))

(defmacro sets (fn a b)
  "like setf, but applies fn to the values"
  `(setf ,a (funcall ,fn ,a ,b)))

(defun ang (a b)
  "Calculates the bearing of a line between a and b"
  (atan (- (y a) (y b))
        (- (x a) (x b))))

(defun split-force (mag ang)
  "Splits the force into its x and y components, returning them as a point"
  (make-instance 'point :x (* mag (cos ang))
                        :y (* mag (sin ang))))

(defun 2body-acel (body1 body2)
  (split-force
   (calc-g (mass body2)
	   (dist (pos body1) (pos body2)))
   (ang (pos body2) (pos body1))))

(defun point+ (&rest points)
  (make-instance 'point
		 :x (apply #'+ (mapcar #'x points))
		 :y (apply #'+ (mapcar #'y points))))

(defun find-acel (body bodies)
  (loop for bod in (remove body bodies)
       let x = ()))

(defun update-pos (body)
  "Updates the position of a body using its velocities"
  (sets #'+ (x (pos body)) (x (vel body)))
  (sets #'+ (y (pos body)) (y (vel body))))

(defun calc-g (M r)
  "Calculates the acceleration due to gravity"
  (/ (* *G* M) (* r r)))

(defun draw-body (body)
  "Draws a body to the screen"
  (sdl:draw-filled-circle
   (pos2pos (pos body)) (/ (mass body) 10) :color sdl:*green*))

(defun sdl-init ()
  (sdl:window (x *screen-size*)
	      (y *screen-size*)
              :title-caption "nbody")
  (setf (sdl:frame-rate) 60))

(defun sdl-main-loop ()
  (sdl:with-events ()
    (:quit-event () t)
    (:key-down-event (:key key)
      (when (sdl:key= key :sdl-key-q)
	(sdl:push-quit-event)))
    
    (:idle ()
	   (when *quit* 
	     (sdl:quit-sdl))
	   (sdl:clear-display sdl:*black*)

           (mapc #'draw-body *bodies*)

	   (sdl:update-display))))

(defun main ()
  (sdl:with-init ()
    (sdl-init)
    (sdl-main-loop)))

(defparameter *bod1* (make-instance 'body :pos (make-instance 'point :x 0 :y 0)
			     :vel (make-instance 'point :x 1 :y 1)
			     :mass 20 
			     :acel (make-instance 'point :x 0 :y 0)))

(defparameter *bod2* (make-instance 'body 
			     :pos (make-instance 'point :x 10000000 :y 10000000)
			     :vel (make-instance 'point :x 1 :y 1)
			     :mass 200 
			     :acel (make-instance 'point :x 0 :y 0)))

(defparameter *bodies* 
  (list *bod1* *bod2*))

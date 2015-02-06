(ql:quickload "lispbuilder-sdl")

(defparameter *G* 6.67e-11)
;(defparameter *screen-size* (make-instance 'point :x 640 :y 480))
(defparameter *screen-size* 640)
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

(defclass velocity (point)
  ((speed :type number
	  :accessor speed
	  :initarg :speed
	  :initform 0)))

(defclass body ()
  ((pos :type point
	:accessor pos
	:initarg :pos
	:initform 0)
   (mass :type number
	 :accessor mass
	 :initarg :mass
	 :initform 0)
   (vel :type velocity
	:accessor vel
	:initarg :vel
	:initform 0)
   (acel :type velocity
	 :accessor acel
	 :initarg :acel
	 :initform 0)))

(defun sdl-init ()
  (sdl:window ;(x *screen-size*)
	      ;(y *screen-size*)
	      640 480
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

           ;;;;;;;;;;;;;;;; add some code here m8 8/8

	   (sdl:update-display))))

(defun main ()
  (sdl:with-init ()
    (sdl-init)
    (sdl-main-loop)))

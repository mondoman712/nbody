(import foreign)

(foreign-declare "#include \"defs.h\"")
(define sdl-window
  (foreign-lambda int makesdlwindow int))

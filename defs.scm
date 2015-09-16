(require-extension bind)
(import foreign)

(bind* "#include \"defs.h\"")
(bind "int sdl-window(int argc, char *argv[]);")

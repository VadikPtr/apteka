(deps
  (archive cc https://github.com/VadikPtr/cc/archive/refs/tags/v1.0.0.tar.gz)
  (archive libuv https://bin.vy.ru.net/libuv-{os}.tar.xz)
  (archive llhttp https://bin.vy.ru.net/llhttp-{os}.tar.xz)
)

(require
  deps/cc/cc-1.0.0
  deps/libuv
  deps/llhttp
)

(project apteka
  (kind exe)
  (files apteka/*.cpp)
  (include apteka)
  (link 'prj cc libuv llhttp)
)

(targets apteka)

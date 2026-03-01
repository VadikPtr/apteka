(deps
  (git cc https://github.com/VadikPtr/cc.git)
  (archive libuv https://bin.vy.ru.net/libuv-{os}.tar.xz)
  (archive llhttp https://bin.vy.ru.net/llhttp-{os}.tar.xz)
  (archive libsodium https://bin.vy.ru.net/libsodium-{os}.tar.xz)
)

(require
  deps/cc
  deps/libuv
  deps/llhttp
  deps/libsodium
)

(project apteka
  (kind exe)
  (files apteka/*.cpp)
  (include apteka)
  (link 'prj cc libuv llhttp libsodium)
)

(targets apteka)

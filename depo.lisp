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

(project aptekalib
  (kind lib)
  (files aptekalib/*.cpp)
  (include 'pub aptekalib)
  (link 'prj 'iface cc libuv llhttp libsodium)
)

(project apteka
  (kind exe)
  (files apteka/*.cpp)
  (include apteka)
  (link 'prj aptekalib)
)

(project hash
  (kind exe)
  (files hash/*.cpp)
  (link 'prj aptekalib)
)

(targets apteka hash)

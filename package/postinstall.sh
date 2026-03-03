#!/bin/sh
set -x

cleanInstall() {
  printf "\033[32m Create user apteka\033[0m\n"
  useradd -r -s /usr/sbin/nologin -d /nonexistent apteka ||:

  printf "\033[32m Reload the service unit from disk\033[0m\n"
  systemctl daemon-reload ||:

  printf "\033[32m Set the enabled flag for the service unit\033[0m\n"
  systemctl enable apteka ||:
  systemctl restart apteka ||:
}

upgrade() {
  printf "\033[32m Post Install of an upgrade\033[0m\n"
}

action="$1"
if [ "$1" = "configure" ] && [ -z "$2" ]
then
  action="install"
elif [ "$1" = "configure" ] && [ -n "$2" ]
then
  action="upgrade"
fi

case "$action" in
  "1" | "install")
    cleanInstall
    ;;
  "2" | "upgrade")
    upgrade
    ;;
  *)
    cleanInstall
    ;;
esac

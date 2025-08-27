#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root."
  exit 1
else
  echo "Installing LaTeX Command Remover (system wide)"
fi

gcc -O2 apply-tex-changes.c readall.c -o apply-tex-changes
mv apply-tex-changes /usr/local/bin

echo Done!

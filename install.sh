#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root."
  exit 1
else
  echo "Installing LaTeX Command Remover (system wide)"
fi

gcc -O2 main.c readall.c -o latex-command-remover
mv latex-command-remover /usr/local/bin
ln -s /usr/local/bin/latex-command-remover /usr/local/bin/apply-tex-changes 

echo Done!

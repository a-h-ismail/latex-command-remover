#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root."
  exit 1
else
  echo "Removing LaTeX Command Remover (system wide)"
fi

rm /usr/local/bin/apply-tex-changes

echo Done!

#!/usr/bin/env bash
set -euo pipefail

# WARNING: YOU NEED TO RUN THE FOLLWOING COMMAND TO LINK JLINK THINGS
# TO THE INSIDE OF THE CONTAINER
# echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1366", MODE="0666"' \
#   | sudo tee /etc/udev/rules.d/99-jlink.rules
# sudo udevadm control --reload-rules && sudo udevadm trigger

# prep neovim folders for lazyvim!
mkdir -p ~/.config/nvim ~/.local/share/nvim ~/.local/state/nvim
xhost +SI:localuser:"$(id -un)" > /dev/null

# create volume if needed
docker volume inspect zephyr-workspace > /dev/null 2>&1 \
  || docker volume create zephyr-workspace

# build container if needed
if ! docker image inspect zephyrcontainer > /dev/null 2>&1; then
  echo "building zephyrcontainer (first run, a few minutes)..."
  docker build -t zephyrcontainer "$(dirname "$0")"
fi
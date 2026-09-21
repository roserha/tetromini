#!/usr/bin/env bash
set -euo pipefail

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
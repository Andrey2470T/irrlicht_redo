#!/bin/bash -e

# Linux build only
install_linux_deps() {
	local pkgs=(
		cmake libpng-dev libjpeg-dev libgl-dev libglew-dev libsdl2-dev libsdl2-image-dev libzstd-dev
	)

	sudo apt-get update
	sudo apt-get install -y --no-install-recommends "${pkgs[@]}" "$@"
}

# macOS build only
install_macos_deps() {
	local pkgs=(
		cmake jpeg-turbo libpng zstd
	)
	export HOMEBREW_NO_INSTALLED_DEPENDENTS_CHECK=1
	export HOMEBREW_NO_INSTALL_CLEANUP=1
	# contrary to how it may look --auto-update makes brew do *less*
	brew update --auto-update
	brew install --display-times "${pkgs[@]}"
	brew unlink $(brew ls --formula)
	brew link "${pkgs[@]}"
}

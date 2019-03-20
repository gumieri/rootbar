#IN DEV
This project is currently under very active development and is not usable in its current state
# Root Bar
Root Bar is a bar for wlroots based wayland compositors such as sway and was designed to address the lack of good bars for wayland.
## Dependencies
	libwayland-dev
	libgtk-3-dev
	libjson-c-dev
	pkg-config
## Building
	hg clone https://bitbucket.org/Scoopta/root-bar rootbar
	cd rootbar/Release
	make
## Installing
	sudo cp rootbar /usr/bin
## Uninstalling
	sudo rm /usr/bin/rootbar

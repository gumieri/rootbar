#IN DEV
This project is at feature parity with Jaybar however it can still be very sensitive to things like configuration mistakes which can cause segfaults
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

#IN DEV
This project is at feature parity with Jaybar however it can still be very sensitive to things like configuration mistakes which can cause segfaults
# Root Bar
Root Bar is a bar for wlroots based wayland compositors such as sway and was designed to address the lack of good bars for wayland.
## Dependencies
	libwayland-dev
	libgtk-3-dev
	libjson-c-dev
	libpulse-dev
	pkg-config
## Building
	hg clone https://hg.sr.ht/~scoopta/rootbar
	cd rootbar/Release
	make
## Installing
	sudo cp rootbar /usr/bin
## Uninstalling
	sudo rm /usr/bin/rootbar
## Bug Reports
Please file bug reports at https://todo.sr.ht/~scoopta/rootbar
## Contributing
Please submit patches to https://lists.sr.ht/~scoopta/rootbar

You can find documentation here https://man.sr.ht/hg.sr.ht/email.md
## Packages
If you're on Arch there's an unofficial AUR package https://aur.archlinux.org/packages/rootbar-hg/

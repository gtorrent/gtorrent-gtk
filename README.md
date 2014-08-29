# gtorrent
[![Build Status](https://travis-ci.org/gtorrent/gtorrent-gtk.svg?branch=master)](https://travis-ci.org/gtorrent/gtorrent-gtk)

*Gtk torrent client built on libtorrent-rasterbar*

## About
A minimalistic lightweight bittorrent client written by some aspies from the 4chan technology board.

## Contributing
### Making changes
1. Fork this repository
2. Make a **new** branch
3. Make your changes
4. Make a pull request to this repo

*As a collaborator*

1. Make a **new branch** on this repository
2. Make your changes
3. Make a pull request
4. (optional) label it "PR: Merging" or "PR: Ready"

### Standards
#### Indentation
We are using tabs for indentations. Fortunately, we are allowed to use
spaces to align code. Will this cause a mess aligning due to different tab
sizes? *Yes.* So let's all agree to use `tabsize=8`.

gTorrent uses astyle to format our code. You can find more about it
[here](http://astyle.sourceforge.net/)

Install astyle, then run the `scripts/format.sh` from the root of the
repo, so that somebody else doesn't have to fix your indents.

#### Coding style
We are using our own, loosely based off kernel style. If something's not
specified here, either follow what others have done or use your better
judgement.

- If/else statements get their opening braces on a newline.
- Functions get their own opening braces.
- Avoid stupid fucking sepples shitty features that aren't usable whenever
  possible.

### Docmentations
Links to the documentation of each library.
- [gtkmm](https://developer.gnome.org/gtkmm/3.12/)
- libtorrent-rasterbar: [Here](http://libtorrent.org/reference.html) and [here](http://libtorrent.org/manual.html).

## Compiling
**Remember, you want libtorrent 1.0.0+**, which your distro probably doesn't
have. You need to compile this from source to have compile gtorrent.

Get the code with:
```
git clone --recursive https://github.com/gtorrent/gtorrent-gtk.git
```
because we have a submodule of gtorrent-core in this repository.

### Dependencies
- cmake (make)
- git (make)
- boost
- glibmm
- gtkmm
- libtorrent-rasterbar
- libnotify

### GNU/Linux or Mac OS X
```
$ mkdir -p build/ && cd build/
$ cmake ..
$ make
```

#### Ubuntu notes
The version of libgtkmm in 14.04 is a bit out of date. To fix this do:

```
$ sudo add-apt-repository ppa:gnome3-team/gnome3-staging
$ sudo apt-get update
$ sudo apt-get dist-upgrade
```

#### ArchLinux
You have it easier, dumbass nerds and your "bleeding edge" repos.
Install from the [AUR](https://aur.archlinux.org/packages/gtorrent/)

This won't be actively maintained until a full release of gtorrent has
been made. Just compile from source.

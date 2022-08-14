# wf-info
A simple wayfire plugin and program to get information from wayfire.

## Build

meson build --prefix=/usr

ninja -C build

sudo ninja -C build install

## Runtime

Enable Information Protocol plugin

Run `wf-info` and click on a window
or, run `wf-info -l` to list information about all windows

## Example

```
$ wf-info
=========================
View ID: 771706616
Client PID: 574934
Workspace: 0,0
App ID: xfce4-terminal
Title: Terminal - root@desktop: /
Role: TOPLEVEL
Geometry: 954,384 843x424
Xwayland: false
=========================
```
# wf-info
A simple wayfire plugin and program to get information from wayfire.

## Build

meson build --prefix=/usr

ninja -C build

sudo ninja -C build install

## Runtime

Enable Wayfire Information Protocol plugin

Run `wf-info` and click on a window

## Example

```
$ wf-info
View ID: 771706616
App ID: xfce4-terminal
Title: Terminal - root@desktop: /
Role: TOPLEVEL
Geometry: 954,384 843x424
```
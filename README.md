# wf-info
A simple wayfire plugin and program to get information from wayfire.

## Build

meson build --prefix=/usr

ninja -C build

sudo ninja -C build install

## Runtime

Enable Information Protocol plugin

Run `wf-info` and click on a window, run `wf-info -l` to list information about all windows, or use `wf-info -i $id` where `$id` is the ID of the view about which you want info. An ID of -1 means the focused view.

## Examples

```
$ wf-info 
=========================
View ID: 1112
Client PID: 1562086
Output: DP-2(ID: 1)
Workspace: 0,0
App ID: python3
Title: Wayfire Window Information
Role: TOPLEVEL
Geometry: 710,231 500x629
Xwayland: false
Focused: false
=========================
```

`python3 src/client/qt/gui.py`

![wf-info-gui](https://github.com/soreau/wf-info/assets/1450125/31d1e550-f145-4cec-a1ab-013b2c57844b)

#!/usr/bin/python3

import os
import sys
from wayfire import WayfireSocket
from wayfire.extra.wpe import WPE

sock = WayfireSocket()
wpe = WPE(sock)

print(f"View Info: {wpe.get_view_info()}")

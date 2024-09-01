#!/usr/bin/python3

import json
from wayfire import WayfireSocket
from wayfire.extra.wpe import WPE

sock = WayfireSocket()
wpe = WPE(sock)

print(f"View Info:\n{json.dumps(wpe.get_view_info(), indent=2, ensure_ascii=False)}")

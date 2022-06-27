wayland_protos = dependency('wayland-protocols')
wayland_server = dependency('wayland-server')
wayland_client = dependency('wayland-client')

wl_protocol_dir = wayland_protos.get_pkgconfig_variable('pkgdatadir')

wayland_scanner = find_program('wayland-scanner')

wayland_scanner_server = generator(
	wayland_scanner,
	output: '@BASENAME@-server-protocol.h',
	arguments: ['server-header', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_client = generator(
	wayland_scanner,
	output: '@BASENAME@-client-protocol.h',
	arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_code = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.c',
	arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

protocols = [
    'wayfire-information.xml'
]

wl_server_protos_src = []
wl_server_protos_headers = []

foreach p : protocols
	xml = join_paths(p)
	wl_server_protos_headers += wayland_scanner_server.process(xml)
	wl_server_protos_src += wayland_scanner_code.process(xml)
endforeach

lib_wl_server_protos = static_library('wl_server_protos', wl_server_protos_src + wl_server_protos_headers,
	dependencies: [wayland_server]) # for the include directory

wf_server_protos = declare_dependency(
	link_with: lib_wl_server_protos,
	sources: wl_server_protos_headers,
)

wl_client_protos_src = []
wl_client_protos_headers = []

foreach p : protocols
	xml = join_paths(p)
	wl_client_protos_headers += wayland_scanner_client.process(xml)
	wl_client_protos_src += wayland_scanner_code.process(xml)
endforeach

lib_wl_client_protos = static_library('wl_client_protos', wl_client_protos_src + wl_client_protos_headers,
	dependencies: [wayland_client]) # for the include directory

wf_client_protos = declare_dependency(
	link_with: lib_wl_client_protos,
	sources: wl_client_protos_headers,
)

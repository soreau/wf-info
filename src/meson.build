sources = ['main.cpp', 'plugin/wayfire-information.cpp']

wf_info = shared_module('wf-info', sources,
    dependencies: [wayfire, wf_server_protos],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))
    
subdir('client')

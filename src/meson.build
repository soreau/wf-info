sources = ['main.cpp', 'wayfire-information/wayfire-information.cpp']

wf_info = shared_module('wf-info', sources,
    dependencies: [wayfire, wf_server_protos],
    install: true, install_dir: join_paths(get_option('libdir'), 'wayfire'))
    
subdir('wf-info')

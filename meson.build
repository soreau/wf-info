project(
	'wf-info',
	'c',
	'cpp',
	version: '0.7.0',
	license: 'MIT',
	meson_version: '>=0.51.0',
	default_options: [
		'cpp_std=c++17',
		'c_std=c11',
	],
)

wayfire = dependency('wayfire')
wlroots = dependency('wlroots')
wfconfig = dependency('wf-config')

add_project_arguments(['-DWLR_USE_UNSTABLE'], language: ['cpp', 'c'])

subdir('metadata')
subdir('proto')
subdir('src')

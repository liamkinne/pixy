project('libpixyusb',
	'cpp',
	version: '0.4'
)

libusb_dep = dependency('libusb-1.0')
thread_dep = dependency('threads')

include_dir = include_directories('include')
common_dir = include_directories('../../common/inc/')

shared_library('libpixyusb',
	'src/chirpreceiver.cpp',
	'src/chirpreceiver.hpp',
	'src/interpreter.hpp',
	'src/pixy.cpp',
	'src/pixyinterpreter.cpp',
	'src/pixyinterpreter.hpp',
	'src/usblink.cpp',
	'src/usblink.h',
	'src/utils/timer.cpp',
	'src/utils/timer.hpp',
	'../../common/src/chirp.cpp',
	include_directories: [
		include_dir,
		common_dir
	],
	dependencies: [
		libusb_dep,
		thread_dep
	],
	cpp_args: [
		'-Wall'
	],
	install: true,
	install_dir: 'lib'
)

pkg = import('pkgconfig')
libs = '-lpixyusb'     # the library/libraries users need to link against
h = ['include'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
	subdirs : h,
	version : '0.4',
	name : 'libpixyusb',
	description : 'C++ API for interfacing with the Pixy camera'
)


#
#	SConstruct file for Sodalis Project
#
#	Fri Apr  7 18:31:23 2006
#		J. Anton, Copyright 2006
#

Help("""
Build the Sodalis Project: 'scons build=... prefix=... use=... mode=... debug=...`
Clear the built Sodalis Project: 'scons -c`
Install the Sodalis Project: 'scons install build=... prefix=... use=... mode=... debug...`
	WARNING: project might be rebuilt firstly

========
Build list: ku2,library,client,server; default: all of them
	'ku2`: build the Kane Utilities 2
	'library`: build the shared client library
		* Kane Utilities will be built automatically
	'client`: build the console client
		* Kane Utilities and
		* shared library will be built automatically
	'server`: build the server
		* Kane Utilities will be built automatically

Install prefix: path, where the project will be installed; default: /usr/local

Special abilities' list; default: gettext
	'gettext`: use gettext library

Build mode: debug|release; default: debug
	'debug`: build debug version, debug mode can be set
	'release`: build the release version, debug mode setting will not be used
	
Debug mode: memory,net,db,func,log; default: all of them
	'memory`: watch memory allocations
	'net`: log all network messages
	'db`: log all SQL requests
	'func`: watch all function calls (entry and exit points)
	'log`: write all log messages on the screen besides the log file
	""")

build=ARGUMENTS.get("build","ku2,library,client,server")
if build!="":
	SConscript("kaneutil2/SConstruct")
if "library" in build or "client" in build:
	SConscript("library/SConstruct")
if "client" in build:
	SConscript("client-console/SConstruct")
if "server" in build:
	SConscript("server/SConstruct")

Alias("install","installserver")
Alias("install","installlibrary")

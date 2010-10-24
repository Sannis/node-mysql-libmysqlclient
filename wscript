####
# Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
# See contributors list in README
#
# See license text in LICENSE file
####

import Options, Utils
from os import unlink, symlink, chdir
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "1.0.3"

def set_options(opt):
  opt.tool_options("compiler_cxx")
  opt.add_option('--mysql-config', action='store', default='mysql_config', help='Path to mysql_config, e.g. /usr/bin/mysql_config')
  opt.add_option('--all', action='store_true', help='Run all tests (include slow, exclude ignored)')
  opt.add_option('--slow', action='store_true', help='Run slow tests')
  opt.add_option('--ignored', action='store_true', help='Run ignored tests')

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  
  # Node.js and debug flags
  conf.env.append_unique('CPPFLAGS', ["-D_FILE_OFFSET_BITS=64","-D_LARGEFILE_SOURCE"])
  conf.env.append_unique('CXXFLAGS', ["-Wall"])
  
  # MySQL flags and libraries
  conf.env.append_unique('CXXFLAGS', Utils.cmd_output(Options.options.mysql_config + ' --include').split())
  conf.env.append_unique('LINKFLAGS', Utils.cmd_output(Options.options.mysql_config + ' --libs_r').split())
  
  if not conf.check_cxx(lib="mysqlclient_r", errmsg="not found, try to find nonthreadsafe libmysqlclient"):
    # link flags are needed to find the libraries
    conf.env.append_unique('LINKFLAGS', Utils.cmd_output(Options.options.mysql_config + ' --libs').split())
    if conf.check_cxx(lib="mysqlclient"):
      conf.env.append_unique('CXXDEFINES', ["MYSQL_NON_THREADSAFE"])
    else:
      conf.fatal("Missing both libmysqlclient_r and libmysqlclient from libmysqlclient-devel or mysql-devel package")
  
  if not conf.check_cxx(header_name='mysql.h'):
    conf.fatal("Missing mysql.h header from libmysqlclient-devel or mysql-devel package")

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "mysql_bindings"
  obj.source = "./src/mysql_bindings.cc ./src/mysql_bindings_connection.cc ./src/mysql_bindings_result.cc ./src/mysql_bindings_statement.cc"
  obj.uselib = "MYSQLCLIENT"

def test(tst):
  if not exists('./tools/nodeunit/bin/nodeunit'):
    print("\033[31mNodeunit doesn't exists.\033[39m\nYou should run `git submodule update --init` before run tests.")
    exit(1)
  else:
    if Options.options.slow and Options.options.ignored:
      Utils.exec_command('node_g ./tools/nodeunit/bin/nodeunit tests/slow tests/ignored')
    else:
      if Options.options.slow:
        Utils.exec_command('node ./tools/nodeunit/bin/nodeunit tests/slow')
      else:
        if Options.options.ignored:
          Utils.exec_command('node ./tools/nodeunit/bin/nodeunit tests/ignored')
        else:
          if Options.options.all:
            Utils.exec_command('node ./tools/nodeunit/bin/nodeunit tests/simple tests/complex tests/slow')
          else:
            Utils.exec_command('node ./tools/nodeunit/bin/nodeunit tests/simple tests/complex')

def lint(lnt):
  # Bindings C++ source code
  print("Run CPPLint:")
  Utils.exec_command('cpplint ./src/*.h ./src/*.cc')
  # Bindings javascript code, tools and tests
  print("Run Nodeint:")
  Utils.exec_command('nodelint ./package.json ./mysql-libmysqlclient.js ./docs/*.js ./tools/*.js ./tools/tests/*.js')

def doc(doc):
  description = ('--desc "MySQL bindings for [Node.js](http://nodejs.org) using libmysqlclient.\n\n' +
                 'Check out the [Github repo](http://github.com/Sannis/node-mysql-libmysqlclient) for the source and installation guide.\n\n' +
                 'Extra information: ')
  ribbon = '--ribbon "http://github.com/Sannis/node-mysql-libmysqlclient" '
  
  downloads = ('--desc-rigth "' +
               'Latest stable v' + VERSION + ':<br/>\n' +
               '<a href="http://github.com/Sannis/node-mysql-libmysqlclient/zipball/v' + VERSION + '">\n' +
               ' <img width="90" src="http://github.com/images/modules/download/zip.png" />\n' +
               '</a>\n' +
               '<a href="http://github.com/Sannis/node-mysql-libmysqlclient/tarball/v' + VERSION + '">\n' +
               ' <img width="90" src="http://github.com/images/modules/download/tar.png" />\n' +
               '</a>" ')
  
  print("Parse README.markdown:")
  Utils.exec_command('dox --title "Node-mysql-libmysqlclient" ' +
                     description +
                     '[ChangeLog](./changelog.html), [API](./api.html), [Examples](./examples.html), [Wiki](http://github.com/Sannis/node-mysql-libmysqlclient/wiki)." ' +
                     ribbon +
                     downloads +
                     '--ignore-filenames ' +
                     './README.markdown ' +
                     '> ./doc/index.html')
  print("Parse CHANGELOG.markdown:")
  Utils.exec_command('dox --title "Node-mysql-libmysqlclient changelog" ' +
                     description +
                     '[Homepage](./index.html), [API](./api.html), [Examples](./examples.html), [Wiki](http://github.com/Sannis/node-mysql-libmysqlclient/wiki)." ' +
                     ribbon +
                     '--ignore-filenames ' +
                     './CHANGELOG.markdown ' +
                     '> ./doc/changelog.html')
  print("Parse API documentation:")
  Utils.exec_command('dox --title "Node-mysql-libmysqlclient API" ' +
                     description +
                     '[Homepage](./index.html), [ChangeLog](./changelog.html), [Examples](./examples.html), [Wiki](http://github.com/Sannis/node-mysql-libmysqlclient/wiki)." ' +
                     ribbon +
                     './mysql-libmysqlclient.js ' +
                     './src/mysql_bindings.cc ' +
                     './src/mysql_bindings_connection.cc ' +
                     './src/mysql_bindings_result.cc ' +
                     './src/mysql_bindings_statement.cc ' +
                     '> ./doc/api.html')
  print("Parse module usage examples:")
  Utils.exec_command('dox --title "Node-mysql-libmysqlclient examples" ' +
                     description +
                     '[Homepage](./index.html), [ChangeLog](./changelog.html), [API](./api.html), [Wiki](http://github.com/Sannis/node-mysql-libmysqlclient/wiki)." ' +
                     ribbon +
                     '--ignore-shabang ' +
                     './doc/examples.js ' +
                     '> ./doc/examples.html')

def gh_pages(context):
  Utils.exec_command('./gh_pages.sh')

def shutdown():
  # HACK to get bindings.node out of build directory.
  # better way to do this?
  t = 'mysql_bindings.node'
  if Options.commands['clean']:
    if exists(t): unlink(t)
  else:
    if exists('build/default/' + t) and not exists(t):
      symlink('build/default/' + t, t)


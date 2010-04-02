import Options, Utils
from os import unlink, symlink, chdir
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.4"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  # http://gist.github.com/349794
  if not conf.check_cfg(package="mysqlclient", args="--cflags --libs", uselib_store="MYSQLCLIENT"):
    if not conf.check_cxx(lib="mysqlclient", uselib_store="MYSQLCLIENT"):
      conf.fatal("Missing libmysqlclient");
  if not conf.check_cxx(header_name='mysql/mysql.h'):
    conf.fatal("Missing libmysqlclient-devel");

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.cxxflags = ["-g", "-D_FILE_OFFSET_BITS=64","-D_LARGEFILE_SOURCE", "-Wall"]
  obj.target = "mysql_bindings"
  obj.source = "./src/mysql_bindings_connection.cc ./src/mysql_bindings_result.cc ./src/mysql_bindings_statement.cc"
  obj.uselib = "MYSQLCLIENT"

def test(tst):
  Utils.exec_command('./tests/run-tests.sh')

def lint(lnt):
  # Bindings C++ source code
  Utils.exec_command('cpplint ./src/*.h ./src/*.cc')
  # Bindings javascript code
  Utils.exec_command('nodelint ./mysql-libmysqlclient.js')
  # Bindings tests
  Utils.exec_command('nodelint ./tests/*.js')
  # Bindings benchmarks
  Utils.exec_command('nodelint ./benchmark/benchmark.js')

def shutdown():
  # HACK to get bindings.node out of build directory.
  # better way to do this?
  t = 'mysql_bindings.node'
  if Options.commands['clean']:
    if exists(t): unlink(t)
  else:
    if exists('build/default/' + t) and not exists(t):
      symlink('build/default/' + t, t)


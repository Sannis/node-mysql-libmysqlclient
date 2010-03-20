import Options
from os import unlink, symlink, chdir, execl
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.2"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  # This doesn't working in my OpenSUSE 11.1 because *.pc file for MySQL doesn't exists
  #if not conf.check_cfg(package='mysqlclient', args='--cflags --libs', uselib_store='MYSQL'):
  #  conf.fatal('Missing mysqlclient');

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "mysql_sync_bindings"
  obj.source = "mysql_sync_bindings.cc"
  # This doesn't working in my OpenSUSE 11.1 because *.pc file for MySQL doesn't exists
  #obj.uselib = "MYSQL"
  obj.lib = "mysqlclient"

def test(tst):
  execl('./tests/test.js')

def shutdown():
  # HACK to get bindings.node out of build directory.
  # better way to do this?
  t = 'mysql_sync_bindings.node'
  if Options.commands['clean']:
    if exists(t): unlink(t)
  else:
    if exists('build/default/' + t) and not exists(t):
      symlink('build/default/' + t, t)


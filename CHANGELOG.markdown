# Changelog

Revision history for node-mysql-libmysqlclient,
the asynchronous MySQL binding for Node.js using libmysqlclient.

## Version 1.3.3

  * Move main library file into /lib/
  * Add QuerySendParallelShouldFail test
  * Fix affectedRowsSync tests, reenable prepare statements tests
  * Fix variable length arrays of non-POD type error with Clang, refs #127

## Version 1.3.2

  * Experimental mysql_send_query() support
  * Fix '\0' handling in query()/querySync() code, fixes #83
  * Add more tests for prepared statements
  * Rewrite README

## Version 1.3.1

  * Fix link against thread-safe libmysqlclient_r
  * Fix FetchAllSyncWithBinaryFields and ThreadSafeSync tests

## Version 1.3.0

  * Improvements:
    * Add binding.gyp sypport for node v0.6.x and newer, refs #111
    * Introduce node_async_shim.h for transparent eio_custom()/uv_queue_work() support
    * Add some new tests with proper CALL results handling, refs #108
    * Merge statements branch withpart of prepared statements supports (unstable)
  * Uncompatible changes:
    * Return BIGINT/DECIMAL/NUMERIC fields as strings to JavaScript land, fixes #110
    * Remove libmysqlclient_r support

## Version 1.2.10

  * Fix bug with wrong 1194 erro handling, #109
  * Add build testing throught Travis CI

## Version 1.2.9

  * Fix test assert for binary buffers
  * Fix tests with latest MySQL, fixes #103
  * Add more tests, refs #102

## Version 1.2.8

  * Fix building with Node.js v0.4.x

## Versions 1.2.5-1.2.7

  * Node.js v0.6.0 support, late 0.5.x support
  * Legacy support for 0.4.x
  * Improve Makefile, fix package.json

## Version 1.2.4

  * Use Date.parse in C++ code to parse date from string to Date object
  * Make compatible with upcoming Node v0.6.x

## Version 1.2.3

  * Make compatible with latest NPM

## Version 1.2.2

  * Refactor mysql_libmysqlclient and mysql_bindings from tests to config.js
  * Remove nodeunit and node-gc submodules
  * Add devDependencies to package.json
  * Remove unnecessary -D_FILE_OFFSET_BITS=64","-D_LARGEFILE_SOURCE, fixed in node_addon.py
  * Fix some lint warnings
  * Fix NPM package size, remove leaktest log

Special thanks to Vitaly Puzrin

## Version 1.2.1

  * Fix wrong gmt_delta calculation in datetime casting, closes #72
  * Revert "build: add check for ev library before adding it to link line"
  * Revert "link libev for 'ev_default_loop_ptr' symbol"

## Version 1.2.0

  * Improvements:
    * Use Buffer(v0.2.x)/FastBuffer(v0.3.x) instead of V8 strings to encode binary data, closes #30
    * Free result object after res.fetchAll() callback, closes #60
    * Link libev for 'ev_default_loop_ptr' symbol
  * Tests improvements:
    * Some refactor of complex tests tables creation
    * Change table type from memory to myisam in tests (for binary fields)

Special thanks to Roman Shtylman
Thanks for Krists Krīgers first buffers implementation, 6826022fb8a34f48d0c4e51a6cd1af487293e89d

## Version 1.1.1

  * Fixes:
    * Fix mysql_options() recall after mysql_real_connect(), closes #71 part 1
    * Fix logic error in res.fetchAll(), closes #71 past 2
  * Improvements:
    * Add --warn flag to node-waf, enables extra compiler warnings, do some fixes
    * Some more lint, JSLint updated

Special thanks to Krists Krīgers

## Version 1.1.0

  * Fixes:
    * Don't ignore connect arguments after any null, closes #63
    * Reset connect_error in MysqlConnection::Close()
    * Fix bug with MYSQL_OPT_RECONNECT reset in MySQL < 5.1.6, closes #66
    * Add MYSQLCONN_MUSTBE_INITIALIZED macro and change error reporting  
      in ConnectSync() and RealConnectSync() to avoid coding errors like in #67
    * Make error reporting for conn.connect() and conn.query() more verbose, closes #68
    * Add error handling for res.fetchAll(), closes #69
    * Fix lint rule in wscript, fix lint errors
  * Improvements:
    * Pass affectedRows and insertId to conn.query() callback
      as object properties, closes #58
    * res.fetchAll*() improvements:
      * res.fetchAll*() takes a boolean option,
        which if true returns array of results, instead of objects
      * Add possibility to use hash with options in res.fetchAll*()
      * Add 'structured' option support for res.fetchall*() methods
      * Pass information about fields as third argument of res.fetchAll() callback, closes #59
      * Add more option constants, update conn.setOptionSync()
      * Add connect flags support and test with select in stored procedure, closes #64
      * Add tests for stored procedures/functions call, see #64
  * Documentation improvements:
    * Add URL shortener example web application
    * Improve gh_pags.sh
  * Refactoring
    * Extract defines from mysql_bindings_connection.h to mysql_bindings.h
    * Extract 'simple' async tests into separate files
    * Split client-realted stuff from conn.getInfoSync() to conn.getClientInfoSync()
  * Update tools/nodeunit from d4949a4 to 200e5a7

Special thanks to Pavel Ponomarenko and Surendra Singhi  
Thanks to Michael Pearson, who pointed out the problem with stored procedures

## Version 1.0.3

  * Update nodeunit from cf2f529 to d4949a4
  * Add HTML documentation into repo
  * Add .npmignore file, closes #54

## Version 1.0.2

  * Tested with Node version v0.2.3
  * Some changes to make the module build with specific libmysqlclient path, closes #50

Special thanks to Benjmain Reesman

## Version 1.0.1

  * Change node-gc to Sannis' fork with some fixes
  * Improve simple.conn.ConnectWithError test, tests cleanup
  * Add gh_pages.sh to simplify docs update
  * Update wscript in accordance to current node_addon.py
  * Fix error reporting in example

Special thanks to Surendra Singhi and http://fallen.habrahabr.ru

## Version 1.0.0

  * Fixes:
    * Rewrite fetchAll function, move V8 stuff to EIO\_After\_FetchAll, closes #34
    * Rewrite MysqlResult::SetFieldValue() procedure to GetFieldValue() function
    * Fix Nodules break, closes #46
    * Return Boolean/throw exception fixes, closes #31
    * Allocate less memory in MysqlConnection::EscapeSync()
    * Add useful macroses to simplify MysqlConnection and MysqlResult
    * Fix lint errors
  * Tests improvements:
    * Refactor tests: split them into simple, complex, slow and ignored
    * Change nodeunit to original caolan's repo
    * Add REPL memory leaks finder
  * Documentation improvements:
    * Add index.html generation from README.markdown
    * Format ChangeLog with markdown
    * Add changelog.html generation from CHANGELOG.markdown
    * Add navigation
    * Add zip- and tarball URLs to index.html
    * Remove docs HTML from repo, you can generate them by 'node-waf doc'
  * Semantic versioning introduced
  * Other changes in README and package.json

Special thanks to Ben Noordhuis

## Version 0.0.10

  * Add JSDocs into bindings code
  * Add API documentation page
  * Add examples.js and Dox-generated examples.html
  * Change caolan/nodeunit to Sannis/nodeunit
  * Update tests to use test.throws()
  * Other cleanup in tests and files

## Version 0.0.9

  * Add MYSQL\_TYPE\_TIME and MYSQL\_TYPE\_SET handling
  * Rename MysqlConn to MysqlConnection
  * Change some methods to read-only properties:
    * MysqlConnection::ConnectErrnoSync()
    * MysqlConnection::ConnectErrorSync()
    * MysqlResult::FieldCountSync()
  * Make it possible to interleave
    sync and async queries on same connection
  * Improve package.json, add 'Dependencies' section into README
  * Rework nested C++ classes into separate
  * Fix lint errors (all, except sscanf)
  * Implemented methods:
    * MysqlConnection::InitSync()
    * MysqlConnection::RealConnectSync()
    * MysqlConnection::SetOptionSync()

Special thanks to Akzhan Abdulin and Surendra Singhi

## Version 0.0.8

  * Make asynchronous connect and fetchAll
  * Tested with Node version v0.2.0
  * Small improves in tests, README
  * Move all TODO to GitHub issues tracker
  * Implemented methods:
    * MysqlConn::Connect() and libeio wrapper
    * MysqlResult::FetchAll() and libeio wrapper

Special thanks to Surendra Singhi

## Version 0.0.7

  * Make asynchronous querying work
  * Simplify javascript logic of createConnection()
  * Allow reconnecting after failed connect
  * Do correct conversion of NULL fields in MysqlResult::AddFieldProperties()
  * Add test for asynchronous queries
  * Rewrite tests config
  * Improve build wscript
  * Implemented methods:
    * MysqlConn::QueryAsync() and libeio wrapper

Special thanks to Surendra Singhi

## Version 0.0.6

  * Update package.json and make small fixes in docs and README
  * Rename MysqlResult::FetchResult method to FetchAll
  * Implemented methods:
    * MysqlResult::DataSeek()
    * MysqlResult::NumRows()
    * MysqlResult::FetchArray()
    * MysqlResult::FetchFields()
    * MysqlResult::FetchField()
    * MysqlResult::FetchFieldDirect()
    * MysqlResult::FetchLengths()
    * MysqlResult::FetchObject()
    * MysqlResult::FieldCount()
    * MysqlResult::FieldSeek()
    * MysqlResult::FieldTell()
    * MysqlResult::Free()
  * MysqlResult class finished

## Version 0.0.5

  * Remove 'Sync' from classes names,
    rename main module file to mysql-libmysqlclient.js
  * Add example MysqlConn::queryAsync() and MysqlConn::async() methods
  * Add a script to verify the existence of unittests for each function
  * Reorganize tests files for new think and add some tests
  * Add speedtest

## Version 0.0.4

  * Split source code into separate files for database connection,
    query result and prepared statements
  * Add nodeunit as the test framework for project
  * Implemented methods:
    * MysqlSyncConn::autoCommit()
    * MysqlSyncConn::commit()
    * MysqlSyncConn::debug()
    * MysqlSyncConn::dumpDebugInfo()
    * MysqlSyncConn::fieldCount()
    * MysqlSyncConn::getInfoString()
    * MysqlSyncConn::getWarnings()
    * MysqlSyncConn::initStmt()
    * MysqlSyncConn::multiMoreResults()
    * MysqlSyncConn::multiNextResult()
    * MysqlSyncConn::multiRealQuery()
    * MysqlSyncConn::realQuery()
    * MysqlSyncConn::rollback()
    * MysqlSyncConn::stat()
    * MysqlSyncConn::storeResult()
    * MysqlSyncConn::threadId()
    * MysqlSyncConn::threadKill()
    * MysqlSyncConn::threadSafe()
    * MysqlSyncConn::useResult()
    * MysqlSyncStmt::prepare()
  * Decrease number of cpplint warnings (dirty hack? maybe)

## Version 0.0.3

  * Add 'lint' target for node-waf
  * Add docs dir, docs/examples.md,
    docs/api\_php\_mysqli.md and docs/api\_node\_mysql\_sync.md
  * Add tests/debug.js for inspecting bindings 'classes' from javascript
  * Implement all methods in 'connection' category:
    * MysqlSyncConn::ChangeUser()
    * MysqlSyncConn::ConnectErrno()
    * MysqlSyncConn::ConnectError()
    * MysqlSyncConn::GetCharset()
    * MysqlSyncConn::GetCharsetName()
    * MysqlSyncConn::Ping()
    * MysqlSyncConn::SelectDb()
    * MysqlSyncConn::SetCharset()
    * MysqlSyncConn::SetSsl()
  * Improve MysqlSyncConn::Connect() functionality, see d2d9ae2

## Version 0.0.2

  * Split MysqlDbSync into MysqlSyncConn and MysqlSyncRes
  * Add unit tests suite
  * Query() API is more clear to PHP MySQLi

## Version 0.0.1

  * Basic functionality: connect/close/query/fenchAll/getInfo/error/etc.
  * Implemented methods:
    * MysqlDbSync::AffectedRows()
    * MysqlDbSync::Connect()
    * MysqlDbSync::Close()
    * MysqlDbSync::Errno()
    * MysqlDbSync::Error()
    * MysqlDbSync::Escape()
    * MysqlDbSync::FetchResult()
    * MysqlDbSync::GetInfo()
    * MysqlDbSync::LastInsertId()
    * MysqlDbSync::Query()
    * MysqlDbSync::WarningCount()


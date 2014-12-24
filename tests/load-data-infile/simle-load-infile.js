var cfg = require('../config.js');
var fs = require('fs');
var createLoadInfileCapableConnection = function (clientClass) {
  var conn = new clientClass();
  conn.initSync();
  conn.setOptionSync(cfg.mysql_libmysqlclient.MYSQL_OPT_LOCAL_INFILE);
  conn.realConnectSync(cfg.host, cfg.user, cfg.password, cfg.database);
  if (conn.connectErrno) {
    console.log("Failed to connect as " + cfg.user + "@" + cfg.host + ": " + conn.connectError);
  }
  return conn;
};
var formatData = function (data, fieldTerminator, rowTerminator) {
  return data.map(function (row) {
    return row.join(fieldTerminator);
  }).join(rowTerminator);
};
var prepareData = function () {
  var i,
    data = [];
  for (i = 0; i < 1000; i++) {
    data.push([i, 'zzzz' + i]);
  }
  return data;
};
var prepareDataFile = function (filePath, data, fieldTerminator, rowTerminator) {
  try {
    if (fs.statSync(filePath)) {
      fs.unlinkSync(filePath);
    }
  } catch (e) {}
  fs.writeFileSync(filePath, formatData(data, fieldTerminator, rowTerminator));
};
var testQueryResult = function (test, conn) {
  var result = conn.querySync("select sum(field1) as sum, count(*) as count from " +
    cfg.test_table);
  result = result && result.fetchAllSync();
  result = result && result[0];
  test.ok(
    result &&
      Number(result.sum) === 1000 * 999 / 2 && //arithmetic progression
      Number(result.count) === 1000,
    "load infile"
  );
};
var createQuery = function (fileName, fieldTerminator, rowTerminator) {
  return 'LOAD DATA LOCAL INFILE \'' + fileName + '\' INTO TABLE ' + cfg.test_table +
    " FIELDS TERMINATED  BY '" + fieldTerminator + "'" +
    " LINES TERMINATED BY '" + rowTerminator + "'";
};
var setUp = function (callback) {
  this.conn = createLoadInfileCapableConnection(this.clientClass);
  this.conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table);
  this.conn.querySync("CREATE TABLE " + cfg.test_table + " (field1 int, field2 text)");
  this.fileName = this.fileName  || 'nothing';
  this.exampleData = prepareData();
  callback();
};
var tearDown =  function (callback) {
  if (this.conn && this.conn.connectedSync()) {
    this.conn.closeSync();
  }
  callback();
};

var syncTest = function (fieldTerminator, rowTerminator, test) {
  test.expect(2);
  if (!this.bufferMode) {
    prepareDataFile(this.fileName, this.exampleData, fieldTerminator, rowTerminator);
  } else {
    this.exampleData = new Buffer(formatData(this.exampleData, fieldTerminator, rowTerminator));
  }
  test.ok(this.conn.querySync(
    createQuery(this.fileName, fieldTerminator, rowTerminator),
    this.bufferMode ? this.exampleData : undefined
  ));
  testQueryResult(test, this.conn);
  test.done();
};
var asyncTest = function (fieldTerminator, rowTerminator, test) {
  test.expect(2);
  var queryCallback =
    function (error) {
      test.ok(error === null, "Asynchronous load data infile fails");
      testQueryResult(test, this.conn);
      test.done();
    }.bind(this),
    callArgs = [createQuery(this.fileName, fieldTerminator, rowTerminator)];
  if (!this.bufferMode) {
    prepareDataFile(this.fileName, this.exampleData, fieldTerminator, rowTerminator);
    callArgs.push(queryCallback);
  } else {
    this.exampleData = new Buffer(formatData(this.exampleData, fieldTerminator, rowTerminator));
    callArgs.push(this.exampleData, queryCallback);
  }
  try {
    this.conn.query.apply(this.conn, callArgs);
  } catch (e) {
    test.ok(false, "Asynchronous load data infile fails");
    test.done();
  }
};
/**
 * Table with tests build as
 * combination of format (CSV/TSV) and
 * syncronous/asyncronous modes of work
 */
var formatModeCombinationsList = {
    syncCSV: function (test) {
      syncTest.call(this, ',',  '\n', test);
    },
    syncTSV: function (test) {
      syncTest.call(this, '\t',  '\n', test);
    },
    asyncCSV: function (test) {
      asyncTest.call(this, ',',  '\n', test);
    },
    asyncTSV: function (test) {
      asyncTest.call(this, '\t',  '\n', test);
    }
};
//enshure mysql server has enabled local_infile option,
//and test has sence at all
module.exports.checkServerOptions = function (test) {
  test.expect(1);
  var conn = createLoadInfileCapableConnection(cfg.mysql_libmysqlclient.MysqlConnectionQueued);
    result = conn.querySync("SHOW VARIABLES LIKE 'local_infile'");
  result = result && result.fetchAllSync();
  result = result && result[0] && result[0].Value;
  test.ok(
    result && result.toLowerCase() === 'on',
    "LOAD INFILE OPERATION REQUIRES local_infile options set on the server\n" +
      "execute 'SET GLOBAL local_infile=ON' in mysql server console"
  );
  test.done();
};
//tests for c++ part
module.exports.bindings = {
  realFile: {
    setUp: function (callback) {
      this.clientClass = cfg.mysql_libmysqlclient.bindings.MysqlConnection;
      this.fileName = '/tmp/load-infile-test.csv';
      this.bufferMode = false;
      setUp.call(this, callback);
    },
    tearDown: tearDown,
    tests: formatModeCombinationsList
  },
  buffer: {
    setUp: function (callback) {
      this.clientClass = cfg.mysql_libmysqlclient.bindings.MysqlConnection;
      this.bufferMode = true;
      setUp.call(this, callback);
    },
    tearDown: tearDown,
    tests: formatModeCombinationsList
  }
};
//tests for queued client
module.exports.queuedClient = {
  realFile: {
    setUp: function (callback) {
      this.clientClass = cfg.mysql_libmysqlclient.MysqlConnectionQueued;
      this.bufferMode = false;
      this.fileName = '/tmp/load-infile-test.csv';
      setUp.call(this, callback);
    },
    tearDown: tearDown,
    tests: formatModeCombinationsList
  },
  buffer: {
    setUp: function (callback) {
      this.clientClass = cfg.mysql_libmysqlclient.MysqlConnectionQueued;
      this.bufferMode = true;
      setUp.call(this, callback);
    },
    tearDown: tearDown,
    tests: formatModeCombinationsList
  }
};
//tests for high-level client
module.exports.highLevel= {
  realFile: {
    setUp: function (callback) {
      this.clientClass = cfg.mysql_libmysqlclient.MysqlConnectionHighlevel;
      this.bufferMode = false;
      this.fileName = '/tmp/load-infile-test.csv';
      setUp.call(this, callback);
    },
    tearDown: tearDown,
    tests: formatModeCombinationsList
  },
  buffer: {
    setUp: function (callback) {
      this.clientClass = cfg.mysql_libmysqlclient.MysqlConnectionHighlevel;
      this.bufferMode = true;
      setUp.call(this, callback);
    },
    tearDown: tearDown,
    tests: formatModeCombinationsList
  }
};

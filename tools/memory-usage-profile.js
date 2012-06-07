#!/usr/bin/env node --expose-gc
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

var
// Require modules
  fs = require("fs"),
  mysql = require("../"),
  async = require("async"),
  helper = require("./memory-helper"),
// Load configuration
  cfg = require("../tests/config"),
// Parameters
  timeLimitInSec = 120 * 60,
  gcLevel = 0,
  connType = 'binding',
  pauseBetweenOperationsInMs = 0,
  dumpMemoryUsageIntervalInMs = 500,
  dumpMemoryUsageInterval = null,
  drawMemoryUsageGraphIntervalInMs = 2 * 60 * 1000,
  drawMemoryUsageGraphInterval = null,
  bufferSize = 16 * 1024,
  bufferString = '',
// Start time
  startHrTime = [0, 0],
// Other
  filenameResult = '',
  filenameData = '',
  dataFd = null;

function dumpMemoryUsage() {
  var
    currentHrTime = process.hrtime(),
    timeDelta = '',
    memoryUsage = process.memoryUsage(),
    Mb = 1024*1024;

  if (currentHrTime[1] >= startHrTime[1]) {
    timeDelta = (currentHrTime[0] - startHrTime[0]) + (currentHrTime[1] - startHrTime[1])/1e9;
  } else {
    timeDelta = (currentHrTime[0] - startHrTime[0] - 1) + (currentHrTime[1] - startHrTime[1] + 1e9)/1e9;
  }

  fs.writeSync(dataFd, timeDelta + '\t' + memoryUsage.rss/Mb + '\t' + memoryUsage.heapUsed/Mb + '\t' + memoryUsage.heapTotal/Mb + "\n");
}

function performOperation(callback) {
  var connection = createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  if (!connection.connectedSync()) {
    setTimeout(callback, pauseBetweenOperationsInMs);
    return;
  }

  // Run series of queries
  async.parallel([
    function (cb) {
      connection.query("SELECT '" + connection.escapeSync(bufferString) + "' AS `str`;", function (error, result) {
        if (error) throw error;

        result.fetchAll(function (error, rows) {
          if (error) throw error;

          rows = null;

          cb(null);
        });
      });
    },
    function (cb) {
      connection.query("INSERT INTO `" + cfg.test_table + "` (`text_data`) VALUES ('" + connection.escapeSync(bufferString) + "');", function (error, info) {
        if (error) throw error;

        info = null;
        cb(null);
      });
    }
  ], function (error) {
    if (error) throw error;

    async.series([
      function (cb) {
        connection.query("SELECT * FROM `" + cfg.test_table + "`;", function (error, result) {
          if (error) throw error;

          result.fetchAll(function (error, rows) {
            if (error) throw error;

            rows = null;

            cb(null);
          });
        });
      },
      function (cb) {
        connection.query("DELETE FROM `" + cfg.test_table + "` WHERE 1;", function (error, info) {
          if (error) throw error;

          info = null;

          cb(null);
        });
      }
    ], function (error) {
      if (error) throw error;

      process.nextTick(callback);
    });
  });
}

// Parse program arguments
params = process.argv.splice(2);

var gcLevelFound = false;
var connTypeFound = false;
params.forEach(function (param) {
  if (param.slice(0, 5) === "--gc=") {
    gcLevel = param.slice(5);
  } else if (param === '--gc') {
    gcLevelFound = true;
  } else if (gcLevelFound) {
    gcLevel = param;
    gcLevelFound = false;
  } else if (param.slice(0, 13) === "--connection=") {
    connType = param.slice(13);
  } else if (param === '--connection') {
    connTypeFound = true;
  } else if (connTypeFound) {
    connType = param;
    connTypeFound = false;
  }
});

gcLevel = parseInt(gcLevel, 10);

var myGc = function () {};
if (gcLevel > 0) {
  myGc = gc;
}
if (gcLevel > 1) {
  myGc = helper.heavyGc;
}

var createConnectionSync = mysql.createConnectionSync;
if (connType === 'queued') {
  createConnectionSync = mysql.createConnectionQueuedSync;
}
if (connType === 'highlevel') {
  createConnectionSync = mysql.createConnectionHighlevelSync;
}

filenameData = "memory-usage-profile.dat";
filenameResult = "memory-usage-profile-connection-" + connType + "-gc-" + gcLevel + ".png";

// Main program
createTestTable();

function createTestTable() {
  console.log("Creating test table...");

  var connection = createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), result = true;

  if (!connection.connectedSync()) {
    throw new Error("Cannot connect to database.");
  }

  result = result && connection.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  result = result && connection.querySync(
    "CREATE TABLE " + cfg.test_table + " (" +
    "  id INT(8) NOT NULL AUTO_INCREMENT, " +
    "  text_data TEXT NULL, " +
    "  PRIMARY KEY(id)" +
    ") " + cfg.store_engine + ";"
  );

  if (!result) {
    throw new Error("Cannot create test table.");
  }

  process.nextTick(openDataFile);
}

function openDataFile() {
  console.log("Opening data file for writing...");

  fs.open(__dirname + "/" + filenameData, "w", function (error, fd) {
    if (error) throw error;

    dataFd = fd;

    process.nextTick(dumpMemoryUsageProfile);
  });
}

function dumpMemoryUsageProfile() {
  console.log("Start writing...");

  // Write data table header to file
  fs.writeSync(dataFd, "time\tRSS\theapUsed\theapTotal\n");

  // Fill test buffer string
  for (var i = 0; i < bufferSize; i++) {
    bufferString += 'X';
  }

  // Start timer, save initial memory usage
  startHrTime = process.hrtime();
  if (dumpMemoryUsageIntervalInMs > 0) {
    dumpMemoryUsageInterval = setInterval(dumpMemoryUsage, dumpMemoryUsageIntervalInMs);
  }
  dumpMemoryUsage();
  if (drawMemoryUsageGraphIntervalInMs > 0) {
    drawMemoryUsageGraphInterval = setInterval(drawMemoryUsageGraph, drawMemoryUsageGraphIntervalInMs);
  }

  // Start async operations
  async.whilst(
    function () {
      var currentHrTime = process.hrtime();
  
      return (currentHrTime[0] - startHrTime[0]) < timeLimitInSec;
    },
    function (callback) {
      performOperation(function () {
        myGc();
  
        if (dumpMemoryUsageIntervalInMs === 0) {
          dumpMemoryUsage();
        }
  
        if (pauseBetweenOperationsInMs > 0) {
          setTimeout(callback, pauseBetweenOperationsInMs);
        } else {
          process.nextTick(callback);
        }
      });
    },
    function (error) {
      if (error) throw error;
  
      // All ok
      if (dumpMemoryUsageInterval) {
        clearInterval(dumpMemoryUsageInterval);
      }
      if (drawMemoryUsageGraphInterval) {
        clearInterval(drawMemoryUsageGraphInterval);
      }
      
      fs.close(dataFd, function (error) {
        if (error) throw error;

        console.log("Draw graph...");
        drawMemoryUsageGraph(function () {
          console.log("Done.");
        });
      });
    }
  );
}

function drawMemoryUsageGraph(callback) {
  require("child_process").exec(
    "gle -output " + __dirname + "/" + filenameResult +  " " + __dirname + "/memory-usage-profile.gle",
    function (error) {
      if (error) throw error;

      if (typeof callback === "function") {
        process.nextTick(callback);
      }
    }
  );
}

#!/usr/bin/env node --expose-gc
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

var
// Require modules
  mysql = require("../"),
  async = require("async"),
  helper = require("./memory-helper"),
// Load configuration
  cfg = require("../tests/config"),
// Parameters
  timeLimitInSec = 10*60*60,
  forceGc = false,
  pauseBetweenOperationsInMs = 1,
  logMemoryUsageIntervalInMs = 400,
  logMemoryUsageInterval = null,
  // Start time
  startHrTime = [0, 0];

function logMemoryUsage() {
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

  console.log(timeDelta + '\t' + memoryUsage.rss/Mb + '\t' + memoryUsage.heapUsed/Mb + '\t' + memoryUsage.heapTotal/Mb);
}

function performOperation(callback) {
  var connection = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  if (!connection.connectedSync()) {
    setTimeout(callback, pauseBetweenOperationsInMs);
    return;
  }

  connection.query("SELECT 'some string' AS `str`;", function (error, result) {
    if (error) throw error;

    result.fetchAll(function (error, rows) {
      if (error) throw error;

      rows = null;

      process.nextTick(callback);
    });
  });
}

// Main program
console.log("time\tRSS\theapUsed\theapTotal");

startHrTime = process.hrtime();
logMemoryUsage();

if (logMemoryUsageIntervalInMs > 0) {
  logMemoryUsageInterval = setInterval(logMemoryUsage, logMemoryUsageIntervalInMs);
}

async.whilst(
  function () {
    var currentHrTime = process.hrtime();

    return (currentHrTime[0] - startHrTime[0]) < timeLimitInSec;
  },
  function (callback) {
    performOperation(function () {
      if (forceGc) {
        helper.heavyGc();
      }

      if (logMemoryUsageIntervalInMs == 0) {
        logMemoryUsage();
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
    if (logMemoryUsageInterval) {
      clearInterval(logMemoryUsageInterval);
    }

    process.exit(0);
  }
);

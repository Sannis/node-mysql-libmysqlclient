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
  timeLimitInSec = 10 * 60,
  gcLevel = 0,
  pauseBetweenOperationsInMs = 1,
  dumpMemoryUsageIntervalInMs = 200,
  dumpMemoryUsageInterval = null,
  drawMemoryUsageGraphIntervalInMs = 60*1000,
  drawMemoryUsageGraphInterval = null,
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

// Parse program arguments
params = process.argv.splice(2);

var gcLevelFound = false;
params.forEach(function (param) {
  if (param.slice(0, 5) === "--gc=") {
    gcLevel = param.slice(5);
  } else if (param === '--gc') {
    gcLevelFound = true;
  } else if (gcLevelFound) {
    gcLevel = param;
    gcLevelFound = false;
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

filenameData = "memory-usage-dump.dat";
filenameResult = "memory-usage-gc-" + gcLevel + ".png";

// Main program
openDataFile();

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
    "gle -output " + __dirname + "/" + filenameResult +  " " + __dirname + "/memory-usage-dump.gle",
    function (error) {
      if (error) throw error;

      if (typeof callback === "function") {
        process.nextTick(callback);
      }
    }
  );
}

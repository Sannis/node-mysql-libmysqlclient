/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

if (require.main === module) {
  console.log("This is helper module and cannot be executed.");
  process.exit(1);
}

function heavyGc() {
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
}

exports.heavyGc = heavyGc;

var initialMemoryUsage = null;
function showMemoryUsageDeltaLine(title, value0, value1) {
  if (value1) {
    process.stdin.write(title + ": " + value1
                      + (value1 > value0 ? " (+" : " (")
                      + (100 * (value1 - value0) / value0).toFixed(2) + "%)\n");
  } else {
    process.stdin.write(title + ": " + value0 + "\n");
  }
}
function showMemoryUsageDelta() {
  if (!initialMemoryUsage) {
    initialMemoryUsage = process.memoryUsage();

    process.stdin.write("Initial memory usage:\n");
    process.stdin.write("rss: "       + initialMemoryUsage.rss       + "\n");
    process.stdin.write("heapUsed: "  + initialMemoryUsage.heapUsed  + "\n");
    process.stdin.write("heapTotal: " + initialMemoryUsage.heapTotal + "\n");
  } else {
    var memoryUsage = process.memoryUsage();

    process.stdin.write("Currect memory usage:\n");
    showMemoryUsageDeltaLine("rss",       initialMemoryUsage.rss,       memoryUsage.rss      );
    showMemoryUsageDeltaLine("heapUsed",  initialMemoryUsage.heapUsed,  memoryUsage.heapUsed );
    showMemoryUsageDeltaLine("heapTotal", initialMemoryUsage.heapTotal, memoryUsage.heapTotal);
  }
}
function resetMemoryUsageDelta() {
  initialMemoryUsage = null;
}

exports.showMemoryUsageDelta = showMemoryUsageDelta;
exports.resetMemoryUsageDelta = resetMemoryUsageDelta;

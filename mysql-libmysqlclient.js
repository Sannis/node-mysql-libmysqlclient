/**
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/**
 * Require bindings native binary
 *
 * @ignore
 */
var binding = require(require('path').join(__dirname, "mysql_bindings"));

/**
 * Create connection to database
 *
 * @param {String} hostname
 * @param {String} user
 * @param {String} password
 * @param {String} database
 * @param {Integer} port
 * @param {String} socket
 * @return {MysqlConnection}
 */
exports.createConnectionSync = function () {
  var db = new binding.MysqlConnection();
  if (arguments.length > 0) {
    db.connectSync.apply(db, Array.prototype.slice.call(arguments, 0, 6));
  }
  return db;
};

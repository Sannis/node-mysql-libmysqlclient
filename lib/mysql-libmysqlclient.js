/*!
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
var bindings = require("./mysql-libmysqlclient-bindings");

/**
 * Create connection to database
 *
 * @param {String|null} hostname
 * @param {String|null} user
 * @param {String|null} password
 * @param {String|null} database
 * @param {Integer|null} port
 * @param {String|null} socket
 * @return {MysqlConnection}
 */
exports.createConnectionSync = function createConnectionSync() {
  var db = new bindings.MysqlConnection();
  if (arguments.length > 0) {
    db.connectSync.apply(db, Array.prototype.slice.call(arguments, 0, 6));
  }
  return db;
};

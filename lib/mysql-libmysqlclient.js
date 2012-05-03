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
var bindings;
try {
  bindings = require('../build/Release/mysql_bindings');
} catch(e) {
  // For node < v0.5.5
  bindings = require('../build/default/mysql_bindings');
}
exports.bindings = bindings;

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
  var connection = new bindings.MysqlConnection();
  if (arguments.length > 0) {
    connection.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }
  return connection;
};

/**
 * MySQL connection with queries queue
 *
 * @class MysqlConnectionQueued
 */
var MysqlConnectionQueued = function MysqlConnectionQueued() {
  // Queries queue
  this._queueBlocked = false;
  this._queue = [];
};

MysqlConnectionQueued.prototype = new bindings.MysqlConnection();

MysqlConnectionQueued.prototype._processQueue = function () {
  if (this._queueBlocked) {
    return;
  }

  if (this._queue.length == 0) {
    return;
  }

  var data = this._queue.shift();
  var query = data[0];
  var callback = data[1];
  data = null;

  var self = this;
  var realCallback = function (error ,result) {
    process.nextTick(function () {
      self._queueBlocked = false;
      self._processQueue();
    });

    if (typeof callback == "function") {
      callback(error, result);
    }
  };

  self._queueBlocked = true;
  bindings.MysqlConnection.prototype.querySend.apply(this, [query, realCallback]);
};

MysqlConnectionQueued.prototype.querySend = function querySend(query, callback) {
  var self = this;

  var realCallback = function (error ,result) {
    process.nextTick(function () {
      self._queueBlocked = false;
      self._processQueue();
    });

    callback(error, result);
  };

  this._queue.push([query, realCallback]);

  this._processQueue();
};

/*!
 * Export MysqlConnectionQueued
 */
exports.MysqlConnectionQueued = MysqlConnectionQueued;

/**
 * Create queued connection to database
 *
 * @param {String|null} hostname
 * @param {String|null} user
 * @param {String|null} password
 * @param {String|null} database
 * @param {Integer|null} port
 * @param {String|null} socket
 * @return {MysqlConnection}
 */
exports.createConnectionQueuedSync = function createConnectionQueuedSync() {
  var connection = new MysqlConnectionQueued();
  if (arguments.length > 0) {
    connection.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }
  return connection;
};

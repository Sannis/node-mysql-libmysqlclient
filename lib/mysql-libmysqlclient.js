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
    // connection.constructor.prototype == bindings.MysqlConnection.prototype;
    bindings.MysqlConnection.prototype.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }

  return connection;
};

/**
 * MySQL connection with queries queue
 *
 * @class MysqlConnectionQueued
 */
var MysqlConnectionQueued = function MysqlConnectionQueued() {
  // Hacky inheritance
  var connection = new bindings.MysqlConnection();
  connection.__proto__ = MysqlConnectionQueued.prototype;

  // Queries queue
  connection._queueBlocked = false;
  connection._queue = [];

  return connection;
};

// Hacky inheritance
MysqlConnectionQueued.prototype = new bindings.MysqlConnection();

/**
 * Process MysqlConnectionQueued internal queue for connect and queries
 */
MysqlConnectionQueued.prototype._processQueue = function () {
  if (this._queueBlocked) {
    return;
  }

  if (this._queue.length == 0) {
    return;
  }

  var data = this._queue.shift();
  var method = data[0];
  var methodArguments = data[1];
  var callback = data[2];
  data = null;

  var self = this;
  var realCallback = function (error, result) {
    process.nextTick(function () {
      self._queueBlocked = false;
      self._processQueue();
    });

    if (typeof callback == "function") {
      callback(error, result);
    }
  };

  methodArguments.push(realCallback);

  self._queueBlocked = true;

  switch (method) {
    case 'connect':
      bindings.MysqlConnection.prototype.connect.apply(this, methodArguments);
      break;
    case 'query':
      bindings.MysqlConnection.prototype.query.apply(this, methodArguments);
      break;
    case 'querySend':
      bindings.MysqlConnection.prototype.querySend.apply(this, methodArguments);
      break;
    default:
      throw new Error("mysql-libmysqlclient internal error: wrong method in queue");
  }
};

/**
 * Connects to the MySQL server
 *
 * @param {String|null} hostname
 * @param {String|null} user
 * @param {String|null} password
 * @param {String|null} database
 * @param {Integer|null} port
 * @param {String|null} socket
 * @param {Integer|null} flags
 * @param {Function(error)} callback
 */
MysqlConnectionQueued.prototype.connect = function query() {
  var callback = arguments[arguments.length - 1];

  var connectArgumentsBegin = Array.prototype.slice.call(arguments, 0, 7);
  while (connectArgumentsBegin.length < 7) {
    connectArgumentsBegin.push(undefined);
  }

  this._queue.push(['connect', connectArgumentsBegin, callback]);

  this._processQueue();
};

/**
 * Performs a query on the database
 *
 * Uses mysql_real_query
 *
 * @param {String} query
 * @param {Function(error, result)} callback
 */
MysqlConnectionQueued.prototype.query = function query(query, callback) {
  this._queue.push(['query', [query], callback]);

  this._processQueue();
};

/**
 * Performs a query on the database
 *
 * Uses mysql_send_query
 *
 * @param {String} query
 * @param {Function(error, result)} callback
 */
MysqlConnectionQueued.prototype.querySend = function querySend(query, callback) {
  this._queue.push(['querySend', [query], callback]);

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
    // connection.constructor.prototype == bindings.MysqlConnection.prototype
    bindings.MysqlConnection.prototype.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }

  return connection;
};

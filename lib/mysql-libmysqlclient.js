/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/*!
 * Require bindings native binary
 */
var bindings = null;
try {
  bindings = require('../build/Release/mysql_bindings');
} catch(e) {
  // For node < v0.5.5
  try {
    bindings = require('../build/default/mysql_bindings');
  } catch(e) {
    bindings = null;
  }
}
if (!bindings) {
  try {
    bindings = require('../build/Debug/mysql_bindings');
  } catch(e) {
    // For node < v0.5.5
    try {
      bindings = require('../build/debug/mysql_bindings');
    } catch(e) {
      bindings = null;
    }
  }
}

/**
 *  == Exports ==
 *
 *  This section contains module exports,
 *  available when you call `require('mysql-libmysqlclient')`.
 **/

/** section: Exports
 * MysqlLibmysqlclient.bindings
 *
 * `mysql-libmysqlclient` module bindings
 **/
/** alias of: MysqlConnection
 * class MysqlLibmysqlclient.bindings.MysqlConnection
 **/
/** alias of: MysqlResult
 * class MysqlLibmysqlclient.bindings.MysqlResult
 **/
/** alias of: MysqlStatement
 * class MysqlLibmysqlclient.bindings.MysqlStatement
 **/
exports.bindings = bindings;

/**
 * MysqlLibmysqlclient
 *
 * Asynchronous MySQL bindings for Node.js
 **/

/** section: Exports
 * MysqlLibmysqlclient.createConnectionSync(hostname[, user[, password[, database[, port[, socket]]]]]) -> MysqlConnection
 *
 * Creates connection to database
 *
 * Synchronous version
 **/
exports.createConnectionSync = function createConnectionSync() {
  var connection = new bindings.MysqlConnection();

  if (arguments.length > 0) {
    // connection.constructor.prototype == bindings.MysqlConnection.prototype;
    bindings.MysqlConnection.prototype.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }

  return connection;
};

/** section: Exports
 * MysqlLibmysqlclient.createConnection(hostname[, user[, password[, database[, port[, socket]]]]][, callback])
 *
 * Creates connection to database
 *
 * Asynchronous version
 **/
exports.createConnection = function createConnection() {
  var connection = new bindings.MysqlConnection();

  var args = Array.prototype.slice.call(arguments);

  // Last argument must be callback function
  var callback = args.pop();
  if (typeof callback != 'function') {
    throw new Error("require('mysql-libmysqlclient').createConnection() must get callback as last argument");
  }

  if (args.length > 0) {
    // Actual callback function
    var actualCallback = function (err) {
      if (err) return callback(err);

      callback(null, connection);
    };

    // + callback
    args.push(actualCallback);

    // connection.constructor.prototype == bindings.MysqlConnection.prototype;
    bindings.MysqlConnection.prototype.connect.apply(connection, args);
  } else {
    // Run callback on next event loop tick
    // For compatibility with MysqlConnection.prototype.connect
    process.nextTick(callback.bind(null /*this*/, null /*err*/, connection));
  }
};

/**
 *  == Classes ==
 *
 *  This section contains module classes API description.
 **/

/** section: Classes
 * class MysqlConnectionQueued < MysqlConnection
 *
 * MySQL connection with queries queue
 **/
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

/*!
 * MysqlConnectionQueued#_processQueue()
 *
 * Process MysqlConnectionQueued internal queue for connect and queries
 **/
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
  var realCallback = function () {
    var args = Array.prototype.slice.call(arguments);

    process.nextTick(function () {
      self._queueBlocked = false;
      self._processQueue();
    });

    if (typeof callback == "function") {
      callback.apply(null, args);
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
 * MysqlConnectionQueued#connect(hostname[, user[, password[, database[, port[, socket]]]]][, callback])
 *
 * Connects to the MySQL server
 **/
MysqlConnectionQueued.prototype.connect = function query() {
  var args = Array.prototype.slice.call(arguments);

  // Last argument should be callback function
  // If not, push it back
  var callback = args.pop();
  if (typeof callback != 'function') {
    args.push(callback);
    callback = null;
  }

  this._queue.push(['connect', args, callback]);

  this._processQueue();
};

/**
 * MysqlConnectionQueued#query(query[, callback])
 *
 * Performs a query on the database
 *
 * Uses mysql_real_query()
 **/
MysqlConnectionQueued.prototype.query = function query(query, callback) {
  this._queue.push(['query', [query], callback]);

  this._processQueue();
};

/**
 * MysqlConnectionQueued#querySend(query[, callback])
 *
 * Performs a query on the database
 *
 * Uses mysql_send_query()
 **/
MysqlConnectionQueued.prototype.querySend = function querySend(query, callback) {
  this._queue.push(['querySend', [query], callback]);

  this._processQueue();
};

/*!
 * Export MysqlConnectionQueued
 */
exports.MysqlConnectionQueued = MysqlConnectionQueued;

/** section: Exports
 * MysqlLibmysqlclient.createConnectionQueuedSync(hostname[, user[, password[, database[, port[, socket]]]]]) -> MysqlConnectionQueued
 *
 * Creates queued connection to database
 *
 * Synchronous version
 **/
exports.createConnectionQueuedSync = function createConnectionQueuedSync() {
  var connection = new MysqlConnectionQueued();

  if (arguments.length > 0) {
    // connection.constructor.prototype == bindings.MysqlConnection.prototype
    bindings.MysqlConnection.prototype.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }

  return connection;
};

/** section: Exports
 * MysqlLibmysqlclient.createConnectionQueued(hostname[, user[, password[, database[, port[, socket]]]]][, callback])
 *
 * Creates queued connection to database
 *
 * Asynchronous version
 **/
exports.createConnectionQueued = function createConnectionQueued() {
  var connection = new MysqlConnectionQueued();

  var args = Array.prototype.slice.call(arguments);

  // Last argument must be callback function
  var callback = args.pop();
  if (typeof callback != 'function') {
    throw new Error("require('mysql-libmysqlclient').createConnectionQueued() must get callback as last argument");
  }

  if (args.length > 0) {
    // Actual callback function
    var actualCallback = function (err) {
      if (err) return callback(err);

      callback(null, connection);
    };

    // + callback
    args.push(actualCallback);

    // connection.constructor.prototype == MysqlConnectionQueued.prototype;
    MysqlConnectionQueued.prototype.connect.apply(connection, args);
  } else {
    // Run callback on next event loop tick
    // For compatibility with MysqlConnection.prototype.connect
    process.nextTick(callback.bind(null /*this*/, null /*err*/, connection));
  }

  return connection;
};

/** section: Classes
 * class MysqlConnectionHighlevel < MysqlConnectionQueued
 *
 * MySQL connection with only high-level methods
 **/
var MysqlConnectionHighlevel = function MysqlConnectionHighlevel() {
  // Hacky inheritance
  var connection = new MysqlConnectionQueued();
  connection.__proto__ = MysqlConnectionHighlevel.prototype;

  connection._queryType = 'query';

  return connection;
};

// Hacky inheritance
MysqlConnectionHighlevel.prototype = new MysqlConnectionQueued();

/**
 * MysqlConnectionHighlevel#setQueryTypeSync(queryType)
 *
 * Sets this._queryType
 **/
MysqlConnectionHighlevel.prototype.setQueryTypeSync = function setQueryTypeSync(queryType) {
  switch (queryType) {
    case 'query':
    case 'querySend':
      this._queryType = queryType;
      break;
    default:
      throw new Error("mysql-libmysqlclient error: wrong queryType passed to connection.setQueryTypeSync");
  }
};

/**
 * MysqlConnectionHighlevel#query(query[, callback])
 *
 * Performs a query on the database
 *
 * Uses mysql_real_query() or mysql_send_query()
 * depends on this._queryType
 **/
MysqlConnectionHighlevel.prototype.query = function query(query, callback) {
  switch (this._queryType) {
    case 'query':
      MysqlConnectionQueued.prototype.query.apply(this, arguments);
      break;
    case 'querySend':
      MysqlConnectionQueued.prototype.querySend.apply(this, arguments);
      break;
    default:
      throw new Error("mysql-libmysqlclient error: wrong this._queryType");
  }
};

/** section: Exports
 * MysqlLibmysqlclient.createConnectionHighlevelSync(hostname[, user[, password[, database[, port[, socket]]]]]) -> MysqlConnectionHighlevel
 *
 * Creates high-level connection to database
 *
 * Synchronous version
 **/
exports.createConnectionHighlevelSync = function createConnectionHighlevelSync() {
  var connection = new MysqlConnectionHighlevel();

  if (arguments.length > 0) {
    // connection.constructor.prototype == bindings.MysqlConnection.prototype;
    MysqlConnectionHighlevel.prototype.connectSync.apply(connection, Array.prototype.slice.call(arguments, 0, 6));
  }

  return connection;
};

/** section: Exports
 * MysqlLibmysqlclient.createConnectionHighlevel(hostname[, user[, password[, database[, port[, socket]]]]][, callback])
 *
 * Creates high-level connection to database
 *
 * Asynchronous version
 **/
exports.createConnectionHighlevel = function createConnectionHighlevel() {
  var connection = new MysqlConnectionHighlevel();

  var args = Array.prototype.slice.call(arguments);

  // Last argument must be callback function
  var callback = args.pop();
  if (typeof callback != 'function') {
    throw new Error("require('mysql-libmysqlclient').createConnection() must get callback as last argument");
  }

  if (args.length > 0) {
    // Actual callback function
    var actualCallback = function (err) {
      if (err) return callback(err);

      callback(null, connection);
    };

    // + callback
    args.push(actualCallback);

    // connection.constructor.prototype == bindings.MysqlConnection.prototype;
    MysqlConnectionHighlevel.prototype.connect.apply(connection, args);
  } else {
    // Run callback on next event loop tick
    // For compatibility with MysqlConnection.prototype.connect
    process.nextTick(callback.bind(null /*this*/, null /*err*/, connection));
  }
};

/*!
 * Export MysqlConnectionQueued
 */
exports.MysqlConnectionHighlevel = MysqlConnectionHighlevel;

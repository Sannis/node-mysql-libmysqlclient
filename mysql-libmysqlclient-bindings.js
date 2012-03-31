/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

try {
  module.exports = require('./build/default/mysql_bindings');
} catch(e) {
  // For node v0.5.5+
  module.exports = require('./build/Release/mysql_bindings');
}

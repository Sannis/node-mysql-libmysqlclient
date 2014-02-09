'use strict';


// 3rd-party
var _ = require('lodash');


////////////////////////////////////////////////////////////////////////////////


module.exports.template = function template(str, data, options) {
  return _.template(str, data, _.extend({interpolate: /\{(.+?)\}/g}, options));
};

/** internal, section: Plugins
 *  Renderers.json(NDoc) -> Void
 *
 *  Registers JSON renderer as `json`.
 *
 *
 *  ##### Example
 *
 *      NDoc.render('json', ast, options);
 *
 *
 *  ##### Options
 *
 *  - **output** (String): File where to output rendered documentation.
 *  - **title** (String): Page title template. You can use `{package.*}`
 *    variables here.
 *    Default: `'{package.name} {package.version} API documentation'`
 **/


'use strict';


// stdlib
var fs = require('fs');


////////////////////////////////////////////////////////////////////////////////

function render_json(ast, options) {
  fs.writeFileSync(
    options.output,
    JSON.stringify({
      title: options.title,
      list: ast.list,
      tree: ast.tree,
      date: (new Date()).toUTCString()
    }, null, '  ')
  );
}

module.exports = function (NDoc) {
  NDoc.registerRenderer('json', render_json);
};

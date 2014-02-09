# NDoc - JavaScript documentation generator

[![Build Status](https://travis-ci.org/nodeca/ndoc.png?branch=master)](https://travis-ci.org/nodeca/ndoc)

NDoc is an inline comment parser and JavaScript documentation generator written
in Node.JS. This project was inspired by [PDoc](http://pdoc.org/syntax.html).
It tries to keep compatibility, but has some differences:

- NDoc is primarily a CLI tool, so you don't need to code your documentor,
  although it can be easily used as [library](#using-ndoc-as-module).
- Clarified EBNF syntax. Definitions now **MUST** be separated with an empty
  line from the following comments.
- Added options for `deprecated` tag: you can set versions, when tag was
  deprecated and when method/property will be removed.
- Added new tags: `read-only`, `internal`, `chainable`
- Events support.


## How to Install

We suppose that you already have `Node` and `npm` installed.
If not - try [nvm](https://github.com/creationix/nvm).

``` bash
npm install -g ndoc
```


## Usage

``` dos
usage: ndoc [-h] [-v] [--exclude PATTERN] [-o PATH] [--use PLUGIN]
            [--alias MAPPING] [-r RENDERER] [--link-format FORMAT]
            [-t TEMPLATE] [--show-all] [--package PACKAGE] [--index FILE]
            [--gh-ribbon URL] [--broken-links ACTION] [--noenv]
            PATH[PATH ...]

Positional arguments:
  PATH                            Source files location

Optional arguments:
  -h, --help                      Show this help message and exit.
  -v, --version                   Show program's version number and exit.
  --exclude PATTERN               Glob patterns of filenames to exclude (you 
                                  can use wildcards: ?, *, **).
  -o PATH, --output PATH          Resulting file(s) location.
  --use PLUGIN                    Load custom plugin.
  --alias MAPPING                 Registers extensions alias. For example 
                                  `cc:js` will register `cc` extension as an 
                                  alias of `js`
  -r RENDERER, --render RENDERER  Documentation renderer (html, json). More 
                                  can be added by custom plugins.
  --link-format FORMAT            View sources link (no links by default) 
                                  format. You can use `{file}` and `{line}` 
                                  and any of `{package.*}` variables for 
                                  interpolation.
  -t TEMPLATE, --title TEMPLATE   Documentation title template. You can use 
                                  any of `{package.*}` variables for 
                                  interpolation. DEFAULT: `{package.name} 
                                  {package.version} API documentation`
  --show-all                      By default `internal` methods/properties 
                                  are not shown. This trigger makes ndoc show 
                                  all methods/properties
  --package PACKAGE               Read specified package.json FILE. When not 
                                  specified, read ./package.json if such file 
                                  exists.
  --index FILE                    Index file (with introduction text), e.g. 
                                  README.md file.
  --gh-ribbon URL                 Add "Fork me on GitHub" ribbon with given 
                                  URL. You can use any of `{package.*}` 
                                  variables for interpolation.
  --broken-links ACTION           What to do if broken link occurred (show, 
                                  hide, throw). DEFAULT: `show`.
  --noenv                         Ignore .ndocrc
```


## Syntax

[NDoc Syntax](https://github.com/nodeca/ndoc/blob/master/syntax.md).
It is similar to [PDoc](https://github.com/tobie/pdoc) one, with some
extentions (see start of this doc for details).


## For developers

You can generate prototype documentation for test:

``` bash
make test
```

Then open `./tests/prototype-doc/index.html`.
Here is [hosted example](http://nodeca.github.com/ndoc/tests/prototype/).


#### Custom parsers and renderers

You can create and use your own parser/renderer via `--use` option. Get one of
the [parsers][parsers] or [renderers][renderers] as a base template, copy it
into separate folder. Create package.json and modify it to fit your needs. Then
attach it with `--use my-module` argument.

[parsers]: https://github.com/nodeca/ndoc/blob/master/lib/ndoc/plugins/parsers
[renderers]: https://github.com/nodeca/ndoc/blob/master/lib/ndoc/plugins/renderers


#### Using NDoc as module

You can use NDoc as module, for example, to override default options processing.

``` javascript
var NDoc = require('ndoc');


var options = {
  linkFormat  : 'http://example.com/{file}#{line}',
  output:     : 'doc'
};

var ast;

try {
  ast = NDoc.parse(['lib/my-module.js'], options);
} catch (err) {
  console.error(err);
  process.exit(1);
}

try {
  NDoc.render('html', ast, options);
} catch (err) {
  console.error(err);
  process.exit(1);
}
```


## License

This project is distributed under [MIT](https://github.com/nodeca/ndoc/blob/master/LICENSE) license.

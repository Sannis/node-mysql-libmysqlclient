Node-mysql-libmysqlclient [![Build status][Build status image]][Build status URL]
=================================================================================

**Asynchronous MySQL binding for [Node.js] using libmysqlclient.**

**This module has been tested with Node.js versions 0.8.15, 0.9.5 and 0.10.13**

[Node.js]: http://nodejs.org/
[Build status image]: https://secure.travis-ci.org/Sannis/node-mysql-libmysqlclient.png?branch=master
[Build status URL]: http://travis-ci.org/Sannis/node-mysql-libmysqlclient


Overview
--------

These bindings provides all general connection/querying functions from the MySQL C API,
and partial support for prepared statements. `Connect`, `query` and `fetchAll` are asynchronous.
This module also includes support for asynchronous `querySend` from internals of `libmysqlclient`.

I started this project in 2010 when Node.js was growing. Ryan had plans to write this binding as part of [GSoC].
It is now used by many projects and has more than 10 contributors,
who are listed in the [AUTHORS](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/AUTHORS) file.
I also maintain the [Node.js MySQL bindings benchmark] which shows how mysql-libmysqlclient performs.

Node-mysql-libmysqlclient's source code is available in the [Github repo] and you can report issues at the project tracker.
Visit the [module site] for API docs and examples. You can also read some extra information in [wiki].

[GSoC]: http://code.google.com/soc/
[Node.js MySQL bindings benchmark]: https://github.com/Sannis/node-mysql-benchmarks
[GitHub repo]: https://github.com/Sannis/node-mysql-libmysqlclient
[module site]: http://sannis.github.com/node-mysql-libmysqlclient
[wiki]: https://github.com/Sannis/node-mysql-libmysqlclient/wiki


Dependencies
------------

To build this module you must install the libmysqlclient library and the development files for it.  
`mysql_config` is used to determine the paths to the library and header files.  
To install these dependencies, execute the commands below for the OS you're running.

For CentOS:

    #> yum install mysql-devel

For openSUSE:

    #> zypper install libmysqlclient-devel

For Debian-based systems/Ubuntu:

    #> apt-get install libmysqlclient-dev

Alternatively, you can use `aptitude` for Debian-based systems.

Please refer to your system's documentation for more information and feel free to send me a patch for this readme.


Installation
------------

You can install this module via NPM:

    $> npm install mysql-libmysqlclient

You can also build latest source code from repository.
Please refer to the developers documentation for more information.


Contributing
------------

This module is written in collaboration with many peoples listed on [GitHub contributors page].
List of [authors ordered by first contribution] also available.

If you are interested in wide MySQL usage in Node.JS applications,
leave your comments to the code.
To contribute any patches, simply fork this repository using GitHub
and send a pull request to [me](https://github.com/Sannis). Thanks!

All information about development use and contribution is placed in the [DEVELOPMENT] file.

[GitHub contributors page]: https://github.com/Sannis/node-mysql-libmysqlclient/graphs/contributors
[authors ordered by first contribution]: https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/AUTHORS
[DEVELOPMENT]: https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/DEVELOPMENT.markdown


Users and related projects
--------------------------

This module is used by [Taobao](http://taobao.com) guys
for their distributed MySQL proxy [Myfox-query](https://github.com/vincent-zhao/Myfox-query-module) module.
There is long time developed Node.js ORM library called [noblerecord](https://github.com/noblesamurai/noblerecord).
It is inspired by Rails and widely used by Noblesamurai.
If you are looking for lightweight Node.js ORM on top of this module,
try [mapper](https://github.com/mgutz/mapper) by Mario Gutierrez.


License
-------

Node-mysql-libmysqlclient itself is published under MIT license.
See license text in [LICENSE](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/LICENSE) file.

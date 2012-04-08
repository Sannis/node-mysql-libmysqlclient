Node-mysql-libmysqlclient [![Build status](https://secure.travis-ci.org/Sannis/node-mysql-libmysqlclient.png?branch=master)](http://travis-ci.org/Sannis/node-mysql-libmysqlclient)
=========================

Asynchronous MySQL binding for [Node.js] using libmysqlclient.  
This module tested with Node.js versions 0.4.12 and 0.6.14, and also with 0.7.7.  

[Node.js]: http://nodejs.org/


Overview
--------

This bindings provides all general connection/querying functions from MySQL C API,
and part of prepared statements support. `Connect`, `query` and `fetchAll` are asynchronous.
This module also includes experimental support for asynchronous `querySend` from internals of `libmysqlclient`.

I start this project in 2010 when Node.js is growing and Ryan had plans to write this binding as part of [GSoC].
Now it is used by many projects and have more than 10 contributors,
listed in [AUTHORS](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/AUTHORS) file.
I am also maintaining [Node.js MySQL bindings benchmark] that shows great performance of this module.

Node-mysql-libmysqlclient source code is available in the [Github repo] and you can place issues at the project tracker.
Visit [module site] for API docs and examples. Also you can read some extra information in [wiki].

[GSoC]: http://code.google.com/soc/
[Node.js MySQL bindings benchmark]: https://github.com/Sannis/node-mysql-bindings-benchmarks
[Github repo]: https://github.com/Sannis/node-mysql-libmysqlclient
[module site]: http://sannis.github.com/node-mysql-libmysqlclient
[wiki]: https://github.com/Sannis/node-mysql-libmysqlclient/wiki


Dependencies
------------

To build this module you must install libmysqlclient library and development files for it.  
`mysql_config` is used to determine the paths to the library and header files.  
To install these dependencies, you can use the command shell.

For CentOS:

    #> yum install mysql-devel

For openSUSE:

    #> zypper install libmysqlclient-devel

For Debian/Ubuntu:

    #> apt-get install libmysqlclient-dev

Yes, you would use aptitude if you want.

Please refer to your system documentation on oner systems and feel free to send me a patch for this readme. 


Installation
------------

You can install this module via NPM:

    $> npm install mysql-libmysqlclient

Also you can build latest source code from repository. This way is described in developers documentation.


Contributing
------------

I will be happy to hear tips from the more experienced programmers.
If you are interested in wide MySQL usage in Node.JS applications,
leave your comments to the code.
I'll be glad to see your forks and commits in them :)

To contribute any patches, simply fork this repository using GitHub
and send a pull request to [me](https://github.com/Sannis). Thanks!

All information about development use and contribution is placed in [DEVELOPMENT](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/DEVELOPMENT.markdown) file.


Contributors
------------

[Authors ordered by first contribution](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/AUTHORS).  
[Full contributors list](https://github.com/Sannis/node-mysql-libmysqlclient/contributors).


License
-------

Node-mysql-libmysqlcleint is published under MIT license.  
See license text in [LICENSE](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/LICENSE) file.

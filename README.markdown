# Node-mysql-libmysqlclient [![Build status](https://secure.travis-ci.org/Sannis/node-mysql-libmysqlclient.png?branch=master)](http://travis-ci.org/Sannis/node-mysql-libmysqlclient) #

###### Asynchronous MySQL binding for [Node.js] using libmysqlclient. ######
__(This module has been tested with Node.js versions 0.4.12, 0.6.19 and 0.7.9.)__

[Node.js]: http://nodejs.org/


Overview
--------

These bindings provides all general connection/querying functions from the MySQL C API,
and part of prepared statements support. `Connect`, `query` and `fetchAll` are asynchronous.
This module also includes experimental support for asynchronous `querySend` from internals of `libmysqlclient`.

I started this project in 2010 when Node.js was growing. Ryan had plans to write this binding as part of [GSoC].
It is now used by many projects and has more than 10 contributors,
who are listed in the [AUTHORS](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/AUTHORS) file.
I also maintain the [Node.js MySQL bindings benchmark] which shows how mysql-libmysqlclient performs.

Node-mysql-libmysqlclient's source code is available in the [Github repo] and you can report issues at the project tracker.
Visit the [module site] for API docs and examples. You can also read some extra information in [wiki].

[GSoC]: http://code.google.com/soc/
[Node.js MySQL bindings benchmark]: https://github.com/Sannis/node-mysql-bindings-benchmarks
[Github repo]: https://github.com/Sannis/node-mysql-libmysqlclient
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

Alternatively, you can use aptitude for Debian-based systems.

Please refer to your system's documentation for more information and feel free to send me a patch for this readme. 


Installation
------------

You can install this module via NPM:

    $> npm install mysql-libmysqlclient

You can also build latest source code from repository.Please refer to the developers documentation for more information.


Contributing
------------

I'm happy to hear tips from the more experienced programmers.
If you are interested in wide MySQL usage in Node.JS applications,
leave your comments to the code.
I'll be glad to see your forks and commits in them :)

To contribute any patches, simply fork this repository using GitHub
and send a pull request to [me](https://github.com/Sannis). Thanks!

All information about development use and contribution is placed in the [DEVELOPMENT](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/DEVELOPMENT.markdown) file.


Contributors
------------

[Authors ordered by first contribution](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/AUTHORS).  
[Full contributors list](https://github.com/Sannis/node-mysql-libmysqlclient/contributors).


License
-------

Node-mysql-libmysqlcleint is published under MIT license.  
See license text in [LICENSE](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/LICENSE) file.

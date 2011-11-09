Node-mysql-libmysqlclient
=========================

MySQL bindings for [Node.js] using libmysqlclient.  
Check out the [Github repo] for the source code.  
Visit [module site] for API docs and examples.  
Extra information available in [wiki].

[Node.js]: http://nodejs.org/
[Github repo]: https://github.com/Sannis/node-mysql-libmysqlclient
[module site]: http://sannis.github.com/node-mysql-libmysqlclient
[wiki]: https://github.com/Sannis/node-mysql-libmysqlclient/wiki


Dependencies
------------

This module tested with Node.js version 0.4.12 (@v1.2.5) and 0.6.0 (@v1.2.7).

To build it you must install libmysqlclient library and development files for it.
Node-waf use mysql_config to determine the paths to the library and header files.
To install these dependencies, you can use the console.

For CentOS:

    #> yum install mysql-devel

For openSUSE:

    #> zypper install libmysqlclient-devel

For Debian/Ubuntu:

    #> apt-get install libmysqlclient-dev

Yes, you would use aptitude if you want.


Installation
------------

You can install this module via NPM:

    $> npm install mysql-libmysqlclient

Also you can build latest source code from repository, see below.


Build and use
-------------

To get source code:

    $> git clone git://github.com/Sannis/node-mysql-libmysqlclient.git
    $> cd node-mysql-libmysqlclient

If you want to build specific version of node-mysql-libmysqlclient, checkout it:

    $> git checkout v1.0.2

To build the bindings you should run:

    $> node-waf configure build

To rebuild:

    $> node-waf distclean configure build

To run tests:

    $> npm install nodeunit
    $> node-waf test

To lint code:

    $> npm install nodelint
    $> # install cpplint.py

The two files required to use these bindings are ./mysql-libmysqlclient.js and
./mysql\_bindings.node (build/default/mysql\_bindings.node).
Put module directory in your NODE_PATH or copy those two files where you need them.


Contributing
------------

I will be happy to hear tips from the more experienced programmers.
If you are interested in wide MySQL usage in Node.JS applications,
leave your comments to the code.
I'll be glad to see your forks and commits in them :)

To contribute any patches, simply fork this repository using GitHub
and send a pull request to [me](https://github.com/Sannis). Thanks!

You can find other information about [contributing and code style guide in wiki](https://github.com/Sannis/node-mysql-libmysqlclient/wiki/contributing).


Contributors
------------

* **Oleg Efimov** ( [E-mail](mailto:efimovov@gmail.com), [GitHub](https://github.com/Sannis), [site](http://sannis.ru) \)

  Author and maintainer.  
  Synchronouse version development.  
  First libeio asynchronouse attempts.  
  Writing tests, docs and so on.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=Sannis).


* **Surendra Singhi** ( [E-mail](mailto:ssinghi@kreeti.com), [GitHub](https://github.com/ssinghi), [GitHub2](https://github.com/kreetitech), [site](http://ssinghi.kreeti.com) \)

  Make asynchronouse connecting and querying work.  
  Fix segmentation faults, partialy closes [#14](https://github.com/Sannis/node-mysql-libmysqlclient/issues/14/find).  
  Some other fixes.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=ssinghi).


* **Akzhan Abdulin** ( [E-mail](mailto:akzhan.abdulin@gmail.com), [GitHub](https://github.com/akzhan) \)

  Tests and code style fixes.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=akzhan).


* **Pavel Ponomarenko** ( [E-mail](mailto:shocksilien@gmail.com), [GitHub](https://github.com/theshock) \)

  Add URL shortener example script. Make good suggestions to improve the functional.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=theshock).


* **Robin Duckett** ( [E-mail](mailto:robin.duckett@gmail.com), [GitHub](https://github.com/robinduckett) \)

  Fix segfault error, partialy closes [#14](https://github.com/Sannis/node-mysql-libmysqlclient/issues/14/find).  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=robinduckett).


* **Roman Shtylman** ( [E-mail](mailto:shtylman@gmail.com), [GitHub](https://github.com/shtylman) \)

  Free result object after fetchAll() callback, closes [#60](https://github.com/Sannis/node-mysql-libmysqlclient/issues/60/find).  
  Link libev for 'ev_default_loop_ptr' symbol.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=shtylman).


* **Dan Beam** ( [E-mail](mailto:dan@danbeam.org), [GitHub](https://github.com/danbeam), [site](http://danbeam.org) \)

  [Simplify javascript logic](https://github.com/Sannis/node-mysql-libmysqlclient/commit/97040c3e7a5d4673ca6f340d8a9bc69da8e398d8) of createConnection().  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=danbeam).


* **Maurits Lamers** ( [E-mail](mailto:maurits@weidestraat.nl), [GitHub](https://github.com/mauritslamers) \)

  Fix a [bug](https://github.com/Sannis/node-mysql-libmysqlclient/commit/c4071181404362b60b9d3a3aed9784b25459fffa) where previous values were set instead of empty.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=mauritslamers).


* **Benjmain Reesman** ( [E-mail](mailto:ben.reesman@gmail.com), [GitHub](https://github.com/benreesman) \)

  Fix [bug](https://github.com/Sannis/node-mysql-libmysqlclient/issues/50/find) with building with libmysqlclient at specific location.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=benreesman).


* **Krists Krīgers** ( [E-mail](mailto:kristsk@cdi.lv), [GitHub](https://github.com/kristsk) \)

  Fix bug with res.fetchAll(), discussed at the end of [#71](https://github.com/Sannis/node-mysql-libmysqlclient/issues/71/find).  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=kristsk).


* **Sergey Novgorodsky** ( [E-mail](mailto:svnovgorodsky@hotmail.com), [GitHub](https://github.com/snovgorodsky) \)

  Fixes for compilation errors.  
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=snovgorodsky).


* **Ștefan Rusu** ( [E-mail](mailto:saltwaterc@gmail.com), [GitHub](https://github.com/SaltwaterC) \)

  Fixes for Node.js 0.4/0.6 compatibility.
  [All commits](https://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=SaltwaterC).


[Full contributors list](https://github.com/Sannis/node-mysql-libmysqlclient/contributors).


License
-------

MIT license. See license text in file [LICENSE](https://github.com/Sannis/node-mysql-libmysqlclient/blob/master/LICENSE).


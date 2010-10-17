Node-mysql-libmysqlclient
=========================

MySQL bindings for [Node.js] using libmysqlclient.  
Check out the [Github repo] for the source code.  
Visit [module site] for API docs and examples.  
Extra information available in [wiki].

[Node.js]: http://nodejs.org/
[Github repo]: http://github.com/Sannis/node-mysql-libmysqlclient
[module site]: http://sannis.github.com/node-mysql-libmysqlclient
[wiki]: http://github.com/Sannis/node-mysql-libmysqlclient/wiki


Dependencies
------------

This module tested with Node version v0.2.3.

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

or (careful!)

    $> wget http://github.com/Sannis/node-mysql-libmysqlclient/tarball/master
    $> tar -xzvf Sannis-node-mysql-libmysqlclient-*.tar.gz
    $> rm -f ./Sannis-node-mysql-libmysqlclient-*.tar.gz
    $> mv Sannis-node-mysql-libmysqlclient-* node-mysql-libmysqlclient
    $> cd node-mysql-libmysqlclient

To build the bindings you should run:

    $> node-waf configure build

To rebuild:

    $> node-waf distclean configure build

To run tests:

    $> git submodule init
    $> git submodule update
    $> node-waf test

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
and send a pull request to [me](http://github.com/Sannis). Thanks!

You can find other information about [contributing and code style guide in wiki](http://github.com/Sannis/node-mysql-libmysqlclient/wiki/contributing).


Contributors
------------

* **Oleg Efimov** ( [E-mail](efimovov@gmail.com), [GitHub](http://github.com/Sannis), [site](http://sannis.ru) \)

  Author and maintainer.  
  Synchronouse version development.  
  First libeio asynchronouse attempts.  
  Writing tests, docs and so on.  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=Sannis).

* **Surendra Singhi** ( [E-mail](<ssinghi@kreeti.com), [GitHub](http://github.com/ssinghi), [GitHub2](http://github.com/kreetitech), [site](http://ssinghi.kreeti.com) \)

  Make asynchronouse connecting and querying work.  
  Fix segmentation faults, partialy closes [#14](http://github.com/Sannis/node-mysql-libmysqlclient/issues/14/find).  
  Some other fixes.  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=ssinghi).

* **Akzhan Abdulin** ( [E-mail](akzhan.abdulin@gmail.com), [GitHub](http://github.com/akzhan) \)

  Tests and code style fixes.  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=akzhan).

* **Robin Duckett** ( [E-mail](robin.duckett@gmail.com), [GitHub](http://github.com/robinduckett) \)

  Fix segfault error, partialy closes [#14](http://github.com/Sannis/node-mysql-libmysqlclient/issues/14/find).  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=robinduckett).
  
* **Dan Beam** ( [E-mail](dan@danbeam.org), [GitHub](http://github.com/danbeam), [site](http://danbeam.org) \)

  [Simplify javascript logic](http://github.com/Sannis/node-mysql-libmysqlclient/commit/97040c3e7a5d4673ca6f340d8a9bc69da8e398d8) of createConnection().  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=danbeam).

* **Maurits Lamers** ( [E-mail](maurits@weidestraat.nl), [GitHub](http://github.com/mauritslamers) \)

  Fix a [bug](http://github.com/Sannis/node-mysql-libmysqlclient/commit/c4071181404362b60b9d3a3aed9784b25459fffa) where previous values were set instead of empty.  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=mauritslamers).

* **Benjmain Reesman** ( [E-mail](ben.reesman@gmail.com), [GitHub](http://github.com/benreesman) \)

  Fix [bug](http://github.com/Sannis/node-mysql-libmysqlclient/commit/46dc9ccf266b39d67ed9e6796a178278ccc153a2) with building with libmysqlclient at specific location.  
  [All commits](http://github.com/Sannis/node-mysql-libmysqlclient/commits/master?author=benreesman).

[Full contributors list](http://github.com/Sannis/node-mysql-libmysqlclient/contributors).


License
-------

MIT license. See license text in file [LICENSE](http://github.com/Sannis/node-mysql-libmysqlclient/blob/master/LICENSE).


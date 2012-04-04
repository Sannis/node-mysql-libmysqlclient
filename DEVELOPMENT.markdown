Node-mysql-libmysqlclient development
=====================================


Build and use
-------------

To get source code:

    $> git clone git://github.com/Sannis/node-mysql-libmysqlclient.git
    $> cd node-mysql-libmysqlclient

If you want to build specific version of node-mysql-libmysqlclient, checkout it:

    $> git checkout v1.3.0

To build the binding using `node-waf` you should run:

    $> make waf

To build the binding using `node-gyp` you should run:

    $> make gyp

To cleanup:

    $> make clean

To run tests:

    $> make test

To lint code:

    $> # install cpplint.py
    $> make lint

The two files required to use these bindings are ./mysql-libmysqlclient.js and
./mysql\_bindings.node (build/default/mysql\_bindings.node).
Put module directory in your NODE_PATH or copy those two files where you need them.


Testing
-------

All functions and possible use cases should have tests, places in one of `tests` sub-folders.
Also `tests/issues` contains tests for some reportes issues and used for regression testing.
`node-mysql-libmysqlclient` uses `nodeunit` as testing tool. If you contributing something,
you should check that your changes do not breakes tests.

If you chnage small part of code you can test it using:

    $> make test

But after all you should run all tests for `node-mysql-libmysqlclient` code by executing:

    $> make test-all

All branches build status: ![Build status](https://secure.travis-ci.org/Sannis/node-mysql-libmysqlclient.png).  
`Master` branch build status: ![Build status](https://secure.travis-ci.org/Sannis/node-mysql-libmysqlclient.png?branch=master).  
[Go to Travis CI site to view tests results](http://travis-ci.org/Sannis/node-mysql-libmysqlclient).


Coding style
------------

Code style based on [Node.js code styles](http://github.com/ry/node/wiki/contributing).

1. Discuss large changes before coding (this is good idea in collaborative development)
2. Javascript code should follow [Douglas Crockford code conventions for the javascript programming language](http://javascript.crockford.com/code.html) and be run through [Nodelint](http://github.com/tav/nodelint). Also:
    * Code should has two space indention
    * Multi-line <code>if</code> statements must have braces
3. C++ code should follow [Google's C++ style guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml) and be run through [cpplint.py](http://google-styleguide.googlecode.com/svn/trunk/cpplint/cpplint.py).
4. All code must be MIT licensed

If you have `cpplint.py` in `$PATH`, you can lint `node-mysql-libmysqlclient` code by executing:

    $> make lint


Contributing
------------

I will be happy to hear tips from the more experienced programmers.
If you are interested in wide MySQL usage in Node.JS applications,
leave your comments to the code.
I'll be glad to see your forks and commits in them :)

You can [email patches to me](mailto:efimovov@gmail.com)
or simply fork this repository using GitHub and send
a pull request to [me](https://github.com/Sannis).  

Thanks!

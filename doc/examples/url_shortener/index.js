/*!
 * Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
 * See contributors list in README
 *
 * See license text in LICENSE file
 */

/**
 * Database schema
 *
 * CREATE TABLE `shortLinks` (
 *   `id` int(11) NOT NULL AUTO_INCREMENT,
 *   `url` varchar(512) NOT NULL,
 *   PRIMARY KEY (`id`)
 * ) DEFAULT CHARSET=utf8;
 */

/*jslint onevar: false */

/**
 * Require Node modules
 */
var http  = require('http');
var url   = require('url');
var fs    = require('fs');

/**
 * Create connection
 */
var conn = require('../../../mysql-libmysqlclient').createConnectionSync();
conn.connectSync('localhost', 'test', '', 'test');

/**
 * Define class for shortened links
 */
function Link(obj) {
  this.setId = function (id) {
    this.id = (isNaN(id) || id <= 0) ? null : parseInt(id, 10);
    return this;
  };
  
  this.getId = function () {
    return this.id;
  };
  
  this.setUrl = function (url) {
    this.url = url || null;
    return this;
  };
  
  this.getUrl = function () {
    return this.url;
  };
  
  this.setCode = function (code) {
    this.id = code ? parseInt(code, 36) : null;
    return this;
  };
  
  this.getCode = function () {
    return this.id ? this.id.toString(36) : null;
  };
  
  this.setId(obj.id).id || this.setCode(obj.code);
  this.setUrl(obj.url);
}

/**
 * Define model for operate with links
 */
function Model(conn) {
  this.create = function (args) {
    return new Link(args);
  };
  
  /**
   * Get link info from database
   */
  this.get = function (link, fn) {
    if (!link.getId()) {
      throw 'EmptyId';
    }
    
    var q = 'SELECT * FROM shortLinks WHERE id = ' + link.getId();
    
    this.conn.query(q, function (err, res) {
      if (err) {
        throw err;
      }
      
      res.fetchAll(function (err, rows) {
        if (err) {
          throw err;
        }
        
        res.freeSync();
        
        fn(rows.length ? link.setUrl(rows[0].url) : null);
      });
    });
  };
  
  /**
   * Put link info to database
   */
  this.put = function (link, fn) {
    var q = 'INSERT INTO shortLinks (id, url) ' +
      'VALUES (NULL, "' + this.conn.escapeSync(link.getUrl()) + '");';
    
    this.conn.query(q, function (err, res) {
      if (err) {
        throw err;
      }
      
      fn(link.setId(
        this.conn.lastInsertIdSync()
      ));
    }.bind(this));
  };
  
  this.conn = conn;
}

/**
 * Create model instance and define renderer
 */
var linkModel = new Model(conn);

var Renderer = {};

/**
 * Function for handle incoming HTTP requests
 */
Renderer.run = function (req, res) {
  var path = url.parse(req.url, true);
  if (path.query && ('add' in path.query)) {
    var addUrl = path.query.add;
    if (!url.parse(addUrl).protocol) {
      addUrl = 'http://' + addUrl;
    }
    this.add(res, addUrl);
  } else if (path.pathname.search(/^\/![0-9a-z]+$/) !== -1) {
    this.send(res, path.pathname.substr(2));
  } else {
    this.index(res);
  }
};

/**
 * 'Add' controller
 */
Renderer.add = function (res, url) {
  res.header(200);
  this.link.put(
    this.link.create({url: url}),
    function (link) {
      res.end(link.getCode());
    }
  );
};

/**
 * 'Send' controller
 */
Renderer.send = function (res, code) {
  this.link.get(
    this.link.create({code: code}),
    function (link) {
      if (link) {
        res.redirect(link.getUrl());
      } else {
        res.header(404, true);
        res.end('There is not such URL');
      }
    }
  );
};

/**
 * 'Index' controller
 */
Renderer.index = function (res) {
  fs.readFile(__dirname + '/index.html', function (err, data) {
    if (err) {
      throw err;
    }
    
    res.header(200);
    res.end(data);
  });
};

/**
 * @ignore
 */
Renderer.link = linkModel;

/**
 * Expand some node classes
 */
http.ServerResponse.prototype.header = function (code, plain) {
  this.writeHead(code, {
    'Content-Type': plain ? 'text/plain' : 'text/html'
  });
};

http.ServerResponse.prototype.redirect =  function (url, status) {
  this.writeHead(status || 302, {
    'Content-Type' : 'text/plain',
    'Location'     : url
  });
  this.write('Redirecting to ' + url);
  this.end();
};

/**
 * Run server
 */
http.createServer(function (req, res) {
  Renderer.run(req, res);
}).listen(12700, "localhost");
console.log('Server running at http://localhost:12700/');


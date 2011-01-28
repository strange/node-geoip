var     inherits = require('util').inherits,
    EventEmitter = require("events").EventEmitter,
      Connection = require('./ceoip').Connection;


exports.createConnection = function() {
  return new GeoIpConnection();
}

function GeoIpConnection() {
  EventEmitter.call(this);
  
  this._queue = [];
  this._currentQuery = null;

  this.connected = false;
  this.closing = false;
  
  this._connection = null;
}

exports.GeoIpConnection = GeoIpConnection;
inherits(GeoIpConnection, EventEmitter);

GeoIpConnection.prototype.connect = function(dbpath) {
  var self = this;
  
  if (this._connection) {
    return;
  }
  
  if (typeof dbpath !== "string" || dbpath.length == 0) {
    throw new Error("Expected dbpath");
  }
  
  this._connection = new Connection();
  
  this._connection.on("connected", function() {
    self.connected = true;
    self._processQueue();
    self.emit("connect");
  });
  
  this._connection.on("closed", function() {
    self.connected = false;
    self.closing = false;
    self._connection = null;
    self.emit("close");
  });
  
  this._connection.on("result", function(result) {
    self._currentQuery[0](result);
    self._currentQuery = null;
    self._processQueue();
  });

  this._connection.on("error", function(error) {
    self.emit("error", error);
  });
  
  try {
    this._connection.connect(dbpath);
  } catch (connectException) {
    process.nextTick(function() {
      self.emit("error", connectException);
    });
  }
}

GeoIpConnection.prototype.query = function(address, callback) {
  this._queue.push([callback, address]);
  this._processQueue();
}

GeoIpConnection.prototype._processQueue = function() {

  if (!this._queue.length || !this._connection || this._currentQuery) {
    return;
  }

  this._currentQuery = this._queue.shift();
  this._connection.query(this._currentQuery[1]);  
}

GeoIpConnection.prototype.close = function() {

  if (this.closing || !this._connection) {
    return;
  }
  
  this.closing = true;
  this._connection.close();
}
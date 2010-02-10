var sys = require('sys'),
  ceoip = require('./ceoip');

function Connection(dbpath) {
  this.con = new ceoip.Connection();
  this.queue = [];
  this.connected = false;
  this.currentQuery = null;

  var self = this;

  this.con.addListener('closed', function() {
    self.connected = false;
    self.emit('closed');
  });

  this.con.addListener('connected', function() {
    self.connected = true;
    self.processQueue();
  });

  this.con.addListener('result', function(result) {
    self.currentQuery[0].emitSuccess(result);
    self.currentQuery = null;
    self.processQueue();
  });

  this.con.connect(dbpath);
}

sys.inherits(Connection, process.EventEmitter);

Connection.prototype.addJob = function(promise, ipAddress) {
  this.queue.push([promise, ipAddress]);
  this.processQueue();
};

Connection.prototype.query = function(ipAddress) {
  var promise = new process.Promise;
  this.addJob(promise, ipAddress);
  return promise;
};

Connection.prototype.processQueue = function () {
  if (!this.queue.length || !this.connected || this.currentQuery) {
    return;
  }
  this.currentQuery = this.queue.shift();
  this.con.query(this.currentQuery[1]);
};

Connection.prototype.close = function () {
  // Emit failure on all promises in queue?
  this.con.close();
};

exports.Connection = Connection;

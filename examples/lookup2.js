var sys = require('sys'),
    geoip = require('../lib/geoip');

var dbpath = '/usr/local/share/GeoIP/GeoLiteCity.dat';
var ip = '216.236.135.152';

sys.puts('Looking up ip: ' + ip + '...\n');
var con = new geoip.Connection(dbpath, 0, function(con) {
  con.query(ip, function(result) {
    for (var attr in result) {
      sys.puts(attr + ' : ' + result[attr]);
    }
    con.close();
  });
});

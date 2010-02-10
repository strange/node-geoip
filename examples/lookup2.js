var sys = require('sys'),
    geoip = require('../lib/geoip');

con = new geoip.Connection('/usr/local/share/GeoIP/GeoLiteCity.dat');

var ip = '216.236.135.152';
sys.puts('Looking up ip: ' + ip + '...\n');
con.query(ip).addCallback(function (result) {
  for (var attr in result) {
    sys.puts(attr + ' : ' + result[attr]);
  }
});

con.close();

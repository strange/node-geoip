var sys = require('sys'),
    geoip = require('../lib/geoip');

con = new geoip.Connection('/usr/local/share/GeoIP/GeoLiteCity.dat');

con.query('216.236.135.152').addCallback(function (result) {
  for (var attr in result) {
    sys.puts(attr + ' : ' + result[attr]);
  }
});

con.close();

var sys = require('sys'),
    ceoip = require('../lib/ceoip');

con = new ceoip.Connection();
con.addListener('connected', function () {
  con.addListener('result', function(result) {
    for (var attr in result) {
      sys.puts(attr + ' : ' + result[attr]);
    }
  });
  con.query('216.236.135.152');
});
con.connect('/usr/local/share/GeoIP/GeoLiteCity.dat');

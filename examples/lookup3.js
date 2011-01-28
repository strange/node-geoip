var sys = require('sys'),
    ceoip = require('../lib/ceoip');

con = new ceoip.Connection();
con.addListener('connected', function () {
  con.addListener('result', function(result) {
    for (var attr in result) {
      sys.puts(attr + ' : ' + result[attr]);
    }
  });
  var ip = '216.236.135.152';
  sys.puts('Looking up ip: ' + ip + '...\n');
  con.query(ip);
});
con.connect('/usr/local/share/GeoIP/GeoLiteCity.dat', ceoip.GEOIP_MMAP_CACHE);

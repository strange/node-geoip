var geoip = require('../lib/geoip');

var dbpath = '/usr/local/share/GeoIP/GeoLiteCity.dat';
var ip = '216.236.135.152';

console.log('Looking up ip: ' + ip + '...\n');

var con = geoip.createConnection();

con.on("error", function(err) {
  console.log(err);
});

con.query(ip, function(result) {
  for (var attr in result) {
    console.log(attr + ' : ' + result[attr]);
  }
  con.close();
});

con.connect(dbpath);

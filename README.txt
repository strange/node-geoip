Naive wrapper around libGeoIP.

1) Install libGeoIP shared libraries.
2) Download GeoLiceCity.dat and place in `/usr/local/share/GeoIP`
2) Run `node-waf configure build`
3) Test `node examples/lookup.js`

(should work out of the box if everything is installed into `/usr/local` --
otherwise tweak example to match your placement of the Geo* database)

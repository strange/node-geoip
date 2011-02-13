Naive wrapper around libGeoIP.

1. Install [libGeoIP] [1] shared libraries.
2. Download [GeoLiteCity.dat] [2] and place it in `/usr/local/share/GeoIP` (see note
   below on file placement).
3. Run `node-waf configure build`.
4. Test `node examples/lookup2.js`.

NOTE: These wrappers will not work with the database that is installed by
default when you install libGeoIP. You must download the city-version of the
database.

NOTE: Everything should work out of the box if the database is placed in
`/usr/local/share/GeoIP` -- otherwise you might need to tweak the examples to
match your placement of the database).

  [1]: http://www.maxmind.com/app/c
     "libGeoIP"
  [2]: http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz
     "GeoLiteCity.dat"

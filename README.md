Naive wrapper around libGeoIP.

1) Install libGeoIP_ shared libraries.
2) Download GeoLiteCity.dat_ and place it in `/usr/local/share/GeoIP` (see note
   below on file placement).
2) Run `node-waf configure build`.
3) Test `node examples/lookup.js`.

NOTE: These wrappers will not work with the database that is installed by
default when you install libGeoIP. You must download the city-version of the
database.

NOTE: Everything should work out of the box if the database is placed in
`/usr/local/share/GeoIP` -- otherwise you might need to tweak the examples to
match your placement of the database)

.. _libGeoIP: http://www.maxmind.com/app/c
.. _GeiLiteCity.dat:
  http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz

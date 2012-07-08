var ceoip = require('../build/Release/ceoip');
var events = require('events');

inherits(ceoip.Connection, events.EventEmitter);
exports.Connection = ceoip.Connection;
exports.GEOIP_STANDARD = ceoip.GEOIP_STANDARD;
exports.GEOIP_MEMORY_CACHE = ceoip.GEOIP_MEMORY_CACHE;
exports.GEOIP_CHECK_CACHE = ceoip.GEOIP_CHECK_CACHE;
exports.GEOIP_INDEX_CACHE = ceoip.GEOIP_INDEX_CACHE;
exports.GEOIP_MMAP_CACHE = ceoip.GEOIP_MMAP_CACHE;

function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}

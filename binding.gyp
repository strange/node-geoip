{
    'conditions': [
        ['OS=="solaris"', {
            'targets': [
                {
                    'target_name': 'ceoip',
                    'sources': [ 'src/ceoip.cc' ],
                    'libraries': [ '-lGeoIP', '-liconv' ],
                    'conditions': [
                        ['target_arch=="ia32"', {
                             'cflags': [ '-fpermissive', '-I/opt/omni/include' ],
                             'ldflags': [ '-L/opt/omni/lib -R/opt/omni/lib' ]
                        }],
                        ['target_arch=="x64"', {
                             'cflags': [ '-fpermissive', '-I/opt/omni/include/amd64' ],
                             'ldflags': [ '-L/opt/omni/lib/amd64 -R/opt/omni/lib/amd64' ]
                        }]
                     ]
                }
            ]
        }],
        ['OS!="solaris"', {
            'targets': [
                {
                    'target_name': 'ceoip',
                    'sources': [ 'src/ceoip.cc' ],
                    'libraries': [ '-lGeoIP' ],
                    'cflags': [ '-I/opt/omni/include' ],
                }
            ]
        }]
    ]
}

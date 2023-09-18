#!/bin/sh
if test ! -e /usr/local/lib/nss_dns.so.1
then
`ldd -f "ln -s  %p /usr/local/lib/nss_dns.so.1\n" ../../src/bed|head -n 1`
fi

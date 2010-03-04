#/usr/bin/sh

echo "NodeJS synchronouse MySQL bindings benchmark:"
./benchmark-sync.js
echo

echo "PHP MySQL bindings benchmark:"
./benchmark.php
echo

#!/usr/bin/bash

./poornames    /usr/bin   > bin.1     2> bin.2
echo "first done"
./poornames -r /usr/bin   > bin-r.1   2> bin-r.2
echo "second done"
./poornames    /usr/lib   > lib.1     2> lib.2
echo "third done"
./poornames -r /usr/lib   > lib-r.1   2> lib-r.2
echo "forth done"
./poornames    /usr/share > share.1   2> share.2
echo "5 done"
./poornames -r /usr/share > share-r.1 2> share-r.2
echo "6 done finished"

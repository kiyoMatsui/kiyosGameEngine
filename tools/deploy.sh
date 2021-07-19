#!/bin/bash

if [ $# -eq 0 ]
then
   echo "need version string as argument, e.g. v1.2" && exit
fi
for i in ./kge/kge*; do
sed -i '1s/^/\/\*-------------------------------\*\\\nCopyright 2020 Kiyo Matsui\nKiyosGameEngine '"$1"' \nApache License\nVersion 2.0, January 2004\nhttp:\/\/www.apache.org\/licenses\/\n\\\*-------------------------------\*\/\n\n/' $i
done

for i in ./examples/*; do
sed -i '1s/^/\/\*-------------------------------\*\\\nCopyright 2020 Kiyo Matsui\nKiyosGameEngine '"$1"' \nApache License\nVersion 2.0, January 2004\nhttp:\/\/www.apache.org\/licenses\/\n\\\*-------------------------------\*\/\n\n/' $i
done

for i in ./test/*; do
sed -i '1s/^/\/\*-------------------------------\*\\\nCopyright 2020 Kiyo Matsui\nKiyosGameEngine '"$1"' \nApache License\nVersion 2.0, January 2004\nhttp:\/\/www.apache.org\/licenses\/\n\\\*-------------------------------\*\/\n\n/' $i
done

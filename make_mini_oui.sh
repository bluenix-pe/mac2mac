#!/bin/sh

echo "Get the latest oui file..."
rm oui.txt
wget http://standards-oui.ieee.org/oui.txt
echo "Converting to mini oui version..."
cat oui.txt | grep "(hex)" | sed 's/  (hex)\t\t//g' > mini.txt
echo "Done"

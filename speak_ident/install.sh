#!/bin/bash


wget http://mistral.univ-avignon.fr/src/ALIZE_3.0.zip
unzip ALIZE_3.0.zip
rm ALIZE_3.0.zip
cd ALIZE_3.0
./configure
make
sudo make install

var=$(pwd)
cd ..


wget http://mistral.univ-avignon.fr/src/LIA_RAL_3.0.zip
unzip LIA_RAL_3.0.zip
rm LIA_RAL_3.0.zip
cd LIA_RAL_3.0
./configure --with-alize=${var}
make
sudo make install

cd ..

sudo apt-get install python-pyaudio

wget http://www.irisa.fr/metiss/guig/spro/spro-4.0.1/spro-4.0.1.tar.gz
tar xvf spro-4.0.1.tar.gz
rm spro-4.0.1.tar.gz
cd spro-4.0
./configure
make
sudo make install
cd ..


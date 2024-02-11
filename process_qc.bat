echo off

set yy=%1
set mm=%2
set dd=%3

node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=2
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=3
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=4
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=5
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=6
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=7
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=8
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=10
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=11
node download_qc.js -y=%yy% -m=%mm% -d=%dd% -c=12

tar xvzf data\%yy%-%mm%-%dd%\caster-2-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-3-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-4-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-5-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-6-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-7-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-8-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-10-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-11-%yy%-%mm%-%dd%-qc.csv.tar.gz
tar xvzf data\%yy%-%mm%-%dd%\caster-12-%yy%-%mm%-%dd%-qc.csv.tar.gz

sum_teqc caster-2-%yy%-%mm%-%dd%-qc.csv caster-3-%yy%-%mm%-%dd%-qc.csv caster-4-%yy%-%mm%-%dd%-qc.csv caster-5-%yy%-%mm%-%dd%-qc.csv caster-6-%yy%-%mm%-%dd%-qc.csv caster-7-%yy%-%mm%-%dd%-qc.csv caster-8-%yy%-%mm%-%dd%-qc.csv caster-10-%yy%-%mm%-%dd%-qc.csv caster-11-%yy%-%mm%-%dd%-qc.csv caster-12-%yy%-%mm%-%dd%-qc.csv


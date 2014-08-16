set nobreak=1
set width=1000
setplot ac1
print mag(inpos-inneg),ph(inpos),mag(out),ph(out) > tmp
quit

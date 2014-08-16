set nobreak=1
set width=1000
setplot tran1
print mag(g),mag(batt)-mag(d) > tmp
quit

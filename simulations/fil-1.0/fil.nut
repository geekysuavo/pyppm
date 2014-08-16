set nobreak=1
set width=1000
setplot ac1
print mag(inp),ph(inp),mag(out),ph(out) > tmp
quit

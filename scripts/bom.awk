BEGIN {
  ref = "";
  fpt = "";
  num = "";
}
/^refdes=/ {
  ref = $2;
}
/^footprint=/ {
  fpt = $2;
}
/^partno=/ {
  num = $2;
}
/^}/ {
  if (ref == part) {
    print "\"" part "\",\"" fpt "\",\"" num "\"";
  }
  ref = "";
  fpt = "";
  num = "";
}

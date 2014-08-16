BEGIN{
  p = 0
  i = 0
  j = 0
}
{
  if ($1 == sprintf("Layer(%d", n)) {
    p = 1
    i = NR + 2
  }
  if (p && $0 ~ /^)/) {
    p = 0
    j = NR - 1
  }
}
END {
  print i, j
}

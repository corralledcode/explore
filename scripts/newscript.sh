PTH=${PTH:-'../bin'}

# running against time
$PTH/flagcalc -r 14 p=0.260000 10 r1 -a s="FORALL (u IN V, v IN V, pct(u,v) > 0) IFF conn1c" all -v crit min rt


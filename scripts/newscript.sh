PTH=${PTH:-'../bin'}

# running against time
$PTH/flagcalc -r 14 p=0.260000 10 r1 -a s="FORALL (u IN V, v IN V, pct(u,v) > 0) IFF conn1c" all -v crit min rt

$PTH/flagcalc -r 10 p=0.500000 100 r1 -a s="FORALL (x IN V, y IN V, ac(x,y) IFF {x,y} ELT E) " all 
# A simple test of set-level operations
$PTH/flagcalc -r 10 p=0.500000 100 r1 -a s="FORALL (x IN V, y IN V, ac(x,y) IFF {x,y} ELT E) " all 

# Testing s1, etc.
$PTH/flagcalc -r 5 p=0.500000 10 r1 -a s1="conn1c" s2="forestc" s4="treec" all 

$PTH/flagcalc -r 5 p=0.500000 10 r1 -a e="SET (u IN V, v IN V, ac(u,v), {u,v})" all -v set crit allsets 

PTH=${PTH:-'../bin'}

$PTH/flagcalc -r 5 p=0.990000 10 r1 -a s1="FORALL (u IN V, v IN V, u != v, \
ac(u,v))" s3="Knc" all 

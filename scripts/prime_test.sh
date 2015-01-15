data="/home/dpavlovic/optimumprime/data/HMO-projekt_instanca_problema.txt"
program="/home/dpavlovic/optimumprime/optimumprime"
out="."

params="--ga-population 60 --ga-iterations 50000 --gr-population 100 --aco-beta 3 \
-aco-fi 0.3 --aco-q0 0.2"

for i in {1..1000}
do

    { $program -i $data $params ; } 2> solution.err

    mv "solution.txt" $out$i"_solution.txt"
    mv "solution.err" $out$i"_solution.err"
done


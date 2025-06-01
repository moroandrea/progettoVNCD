#!/bin/bash
RESULT_FOLDER="../results"
for sca in $(ls $RESULT_FOLDER/*sca) 
do
    out=${sca/sca/csv}
    opp_sca2csv.pl -F generatedWSMs $sca > $out
done

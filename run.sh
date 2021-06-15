ks=(5 10 100)
n=6
array=()
i=0
if [ $1 = 'parallel' ]; then 
    nws=(2 4)
else
    nws=(-1)
fi

for k in "${ks[@]}"
    do for nw in "${nws[@]}"
        do if [ $nw -lt 0 ]; then
            MY_VAR="$(./$1 $k $2 $3)"
        else
            MY_VAR="$(./$1 $k $nw $2 $3)"
        fi
        array[$i]=$MY_VAR
        ((i+=1))
    done 
done


while [ $n -gt 0 ]
do
    num=${array[`expr $n - 1`]}    
    # add them to sum
    echo ("$sum + $num") | bc
    # decrement count of n
    n=`expr $n - 1`
done
echo 'a'
avg=`echo "$sum / $m" | bc -l`
printf '%0.3f' "$avg"


exit 0
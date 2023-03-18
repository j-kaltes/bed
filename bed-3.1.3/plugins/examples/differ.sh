for i in $@
do
diff "$i" "/home/jka/bed-0.2.26/plugins/examples/$i"
done

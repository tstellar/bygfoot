set -e

bygfoot_bin=$1
bygfoot_bindir=`dirname $bygfoot_bin`

json_file=`mktemp`
save_dir=`mktemp -d`

cat <<EOF > $json_file
{ 'commands' : [
  { 'add_country' : { 'name' : 'faroe_islands' }},
  { 'start_bygfoot' : {}},
  { 'simulate_games' : {'weeks' : 5}},
  { 'save_bygfoot' : {'filename' : '$save_dir/save0.zip'}},
  { 'load_bygfoot' : {'filename' : '$save_dir/save0.zip'}},
  { 'save_bygfoot' : {'filename' : '$save_dir/save1.zip'}},
  { 'simulate_games' : {'years' : 1}},
  { 'save_bygfoot' : {'filename' : '$save_dir/save2.zip'}},
  { 'load_bygfoot' : {'filename' : '$save_dir/save2.zip'}},
  { 'save_bygfoot' : {'filename' : '$save_dir/save3.zip'}},
]}
EOF

echo $save_dir
tmphome=`mktemp -d`
pushd $bygfoot_bindir
HOME=$tmphome ./bygfoot --random-seed=1 --json=$json_file

for f in save0 save1 save2 save3; do

    mkdir -p $save_dir/$f
    unzip -q $save_dir/$f.zip -d $save_dir/$f
    sed -i "s/${f}___//g" $save_dir/$f/*
    for file in `ls $save_dir/$f`; do
        new_name=`echo $file | sed "s/${f}___//g"`
        mv $save_dir/$f/$file $save_dir/$f/$new_name
    done
done
diff -r $save_dir/save0 $save_dir/save1
diff -r $save_dir/save2 $save_dir/save3

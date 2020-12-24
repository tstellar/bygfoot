
bygfoot_bin=$1
bygfoot_bindir=`dirname $bygfoot_bin`

status=0
countries=" \
    albania \
    andorra \
    armenia \
    austria \
    azerbaijan \
    belarus \
    belgium \
    bosnia_herzegovina \
    bulgaria \
    croatia \
    cyprus \
    czech \
    denmark \
    england \
    estonia \
    faroe_islands \
    finland \
    france \
    n_macedonia \
    georgia \
    germany \
    gibraltar \
    greece \
    hungary \
    iceland \
    ireland \
    israel \
    italy \
    kazakhstan \
    kosovo \
    latvia \
    liechtenstein \
    lithuania \
    luxembourg \
    malta \
    moldova \
    montenegro \
    n_ireland \
    netherlands \
    norway \
    poland \
    portugal \
    romania \
    russia \
    san_marino \
    scotland \
    serbia \
    slovakia \
    slovenia \
    spain \
    sweden \
    switzerland \
    turkey \
    ukraine \
    wales"

json_file=`mktemp`

pushd $bygfoot_bindir

for c in $countries; do
    cat <<EOF > $json_file
{ 'commands' : [
  { 'add_country' : { 'name' : '$c' }},
  { 'start_bygfoot' : { }},
  { 'simulate_games' : {'seasons' : 2 }},
]}
EOF

    tmphome=`mktemp -d`
    if HOME=$tmphome ./bygfoot --random-seed=1 --json=$json_file; then
        echo "$c: PASS"
    else
        echo "$c: FAIL"
        status=1
    fi
done


exit $status

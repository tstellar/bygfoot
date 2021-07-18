#!/bin/bash

set -e

output_dir=`pwd`

while [ $# -gt 0 ]; do
    case $1 in
        --all)
            verify=1
            benchmark=1
            profile=1
            ;;
        --verify)
            verify=1
            ;;
        --benchmark)
            benchmark=1
            ;;
        --profile)
            profile=1
            ;;
        --output-dir)
            shift
            output_dir=$1
            ;;
	--fast)
	    input_suffix="-fast"
	    ;;
        *)
            bygfoot_bin=$1
            ;;
    esac
    shift
done

if [ -z "$verify$profile" ]; then
    benchmark=1
fi

mkdir -p $output_dir
bygfoot_bindir=`dirname $bygfoot_bin`
input_dir=`pwd`/`dirname "$0"`

pushd $bygfoot_bindir

tmphome=`mktemp -d`
export HOME=$tmphome

set -x

if [ -n "$verify" ]; then
    ./bygfoot --random-seed=1 --json=$input_dir/verify$input_suffix.json &> $output_dir/verify.log
    gzip -n $output_dir/verify.log
fi

if [ -n "$benchmark" ]; then
    perf stat -B ./bygfoot --random-seed=1 --json=$input_dir/benchmark$input_suffix.json &> $output_dir/benchmark.log
fi

if [ -n "$profile" ]; then
    perf record -g -o $output_dir/perf.data ./bygfoot --random-seed=1 --json=$input_dir/benchmark$input_suffix.json 
fi

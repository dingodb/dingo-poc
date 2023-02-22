#!/bin/bash

mydir="${BASH_SOURCE%/*}"
if [[ ! -d "$mydir" ]]; then mydir="$PWD"; fi
. $mydir/shflags



DEFINE_string role 'store' 'server role'

BASE_DIR=$(dirname $(cd $(dirname $0); pwd))
DIST_DIR=$BASE_DIR/dist

SERVER_NUM=3

function start_program() {
  role=$1
  root_dir=$2
  echo "start server: ${root_dir}"

  cd ${root_dir}
  
  nohup ./bin/dingodb_server --role ${role}  --conf ./conf/${role}.yaml 2>&1 >./log/out &
}


for ((i=1; i<=$SERVER_NUM; ++i)); do
  program_dir=$BASE_DIR/dist/${FLAGS_role}${i}
  start_program ${FLAGS_role} ${program_dir}
done

echo "Finish..."

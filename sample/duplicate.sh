source=${1:-idle}
target=${2:-copy}

mkdir -p ${target}

(
  cd ${target}
  sed s/${source}/${target}/g ../${source}/${source}.c    > ./${target}.c
  sed s/${source}/${target}/g ../${source}/${source}.cdl  > ./${target}.cdl
  sed s/${source}/${target}/g ../${source}/${source}.cfg  > ./${target}.cfg
  sed s/${source}/${target}/g ../${source}/${source}.h    > ./${target}.h
  . ../config.sh ${target}
)

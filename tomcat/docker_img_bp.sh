#!/bin/bash

if [ "$3" ]; then
    DOCKER_ACC=lhdamiani
    DOCKER_REPO=tomcat_docker
    IMG_TAG=dev
 else
    DOCKER_ACC=$1
    DOCKER_REPO=$2
    IMG_TAG=$3
 fi

docker build -t $DOCKER_ACC/$DOCKER_REPO:$IMG_TAG .
docker push $DOCKER_ACC/$DOCKER_REPO:$IMG_TAG

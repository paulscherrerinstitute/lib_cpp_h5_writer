#!/bin/bash
# remove previously existing image
ID=$(docker images | grep lhdamiani/tomcat_writer | grep dev | awk '{print $3}')
if [[ ! -z "$ID" ]]
then
    docker rmi $ID
fi
# build and tag
docker build --tag lhdamiani/tomcat_writer:dev .

# runs test script on a new container
docker run --rm lhdamiani/tomcat_writer:dev

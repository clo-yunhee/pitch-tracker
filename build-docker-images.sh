#!/bin/bash

cd docker
for tag in linux windows macos android; do
    docker build --rm -t clorika/ptbuilder:$tag . -f Dockerfile.$tag
    docker tag clorika/ptbuilder:$tag docker.io/clorika/ptbuilder:$tag
    docker push clorika/ptbuilder:$tag
done

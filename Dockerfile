FROM objectcomputing/opendds

MAINTAINER siddharth.menon@mathworks.com

RUN apt-get update && apt-get install -y \
    cmake

ADD . /opt/OpenDDS_Example

WORKDIR /opt/OpenDDS_Example

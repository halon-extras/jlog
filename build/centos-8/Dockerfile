FROM --platform=linux/amd64 rockylinux:8
LABEL org.opencontainers.image.authors="support@halon.io"

ARG HALON_REPO_USER
ARG HALON_REPO_PASS

RUN yum update -y && yum install -y epel-release

RUN echo "[halon]" >> /etc/yum.repos.d/Halon.repo
RUN echo "name=Halon repository" >> /etc/yum.repos.d/Halon.repo
RUN echo "baseurl=https://${HALON_REPO_USER}:${HALON_REPO_PASS}@repo.halon.io/dists/rhel8/amd64/stable" >> /etc/yum.repos.d/Halon.repo
RUN echo "gpgcheck=1" >> /etc/yum.repos.d/Halon.repo
RUN echo "gpgkey=https://raw.githubusercontent.com/halon/changelog/master/pgp-keys/7F0A73B5.asc" >> /etc/yum.repos.d/Halon.repo
RUN echo "enabled=1" >> /etc/yum.repos.d/Halon.repo
RUN yum install -y halon-5.10.0

RUN yum install -y cmake3 make gcc-c++ rpm-build autoconf
RUN mkdir /tmp/halon
RUN cd /tmp/halon \
    && curl -O -L https://github.com/omniti-labs/jlog/archive/refs/tags/2.5.3.tar.gz \
    && tar -zxvf 2.5.3.tar.gz && cd jlog-2.5.3 \
    && autoconf && CFLAGS=-fPIC ./configure && make && make install \
    && mkdir -p /jlog && cp LICENSE /jlog/LICENSE
RUN rm -rf /tmp/halon
RUN echo -n "CENTOS8" > /OSRELEASE.txt

COPY build.sh /build.sh
CMD ["/build.sh"]

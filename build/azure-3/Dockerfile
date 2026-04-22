FROM --platform=linux/amd64 mcr.microsoft.com/azurelinux/base/core:3.0
LABEL org.opencontainers.image.authors="support@halon.io"

ARG TARGETARCH
ARG TARGETOS

ARG HALON_REPO_USER
ARG HALON_REPO_PASS

RUN tdnf install -y ca-certificates azurelinux-repos-extended

RUN echo "[halon]" >> /etc/yum.repos.d/Halon.repo
RUN echo "name=Halon repository" >> /etc/yum.repos.d/Halon.repo
RUN echo "baseurl=https://${HALON_REPO_USER}:${HALON_REPO_PASS}@repo.halon.io/dists/azure3/${TARGETARCH}/stable" >> /etc/yum.repos.d/Halon.repo
RUN echo "gpgcheck=1" >> /etc/yum.repos.d/Halon.repo
RUN echo "gpgkey=https://raw.githubusercontent.com/halon/changelog/master/pgp-keys/7F0A73B5.asc" >> /etc/yum.repos.d/Halon.repo
RUN echo "enabled=1" >> /etc/yum.repos.d/Halon.repo
RUN tdnf install -y halon-6.7.0

RUN tdnf install -y cmake make build-essential rpm-build
RUN mkdir /tmp/halon
RUN cd /tmp/halon \
    && curl -O -L https://github.com/omniti-labs/jlog/archive/refs/tags/2.5.3.tar.gz \
    && echo "66730afc62aa9c9f93ef686998a5396f8721edca3750097d4a2848a688d55bf9 2.5.3.tar.gz" | sha256sum -c \
    && tar -zxvf 2.5.3.tar.gz && cd jlog-2.5.3 \
    && autoconf && CFLAGS=-fPIC ./configure && make && make install \
    && mkdir -p /jlog && cp LICENSE /jlog/LICENSE
RUN rm -rf /tmp/halon
RUN echo -n "AZURE3" > /OSRELEASE.txt

COPY build.sh /build.sh
CMD ["/build.sh"]

FROM ubuntu@sha256:bce129bec07bab56ada102d312ebcfe70463885bdf68fb32182974bd994816e0

RUN sed -i "s/http:\/\/archive.ubuntu.com/http:\/\/mirrors.bupt.edu.cn\/ubuntu/g" /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y lib32z1 xinetd && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    useradd -m ctf && \
    cp -R /usr/lib* /home/ctf && \
    mkdir /home/ctf/dev && \
    mknod /home/ctf/dev/null c 1 3 && \
    mknod /home/ctf/dev/zero c 1 5 && \
    mknod /home/ctf/dev/random c 1 8 && \
    mknod /home/ctf/dev/urandom c 1 9 && \
    chmod 666 /home/ctf/dev/* && \
    mkdir /home/ctf/bin && \
    cp /bin/sh /home/ctf/bin && \
    cp /bin/ls /home/ctf/bin && \
    cp /bin/cat /home/ctf/bin && \
    echo "Blocked by ctf_xinetd" > /etc/banner_fail

COPY ./ctf.xinetd /etc/xinetd.d/ctf
COPY ./start.sh /start.sh
COPY ./bin/ /home/ctf/

RUN chmod +x /start.sh && \
    chown -R root:ctf /home/ctf && \
    chmod -R 750 /home/ctf 

CMD ["/start.sh"]

EXPOSE 9999

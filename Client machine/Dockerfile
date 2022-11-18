FROM ubuntu
RUN apt-get update && \
apt install -y iproute2 && \
apt-get install -y tcpdump
RUN mkdir -p /var/lib/
COPY client  /var/lib/

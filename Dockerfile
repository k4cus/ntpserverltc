FROM alpine:latest
MAINTAINER www.vberry.net <office [at] vberry.net>
LABEL maintainer="k4cus"


COPY docker/ntpServerLTC /opt
COPY docker/run.sh ./

RUN apk update && \
    apk add --no-cache tzdata

EXPOSE 123/udp

RUN chmod +x run.sh
CMD ["./run.sh"]

FROM alpine:latest
RUN mkdir /service
COPY runnableService /service
WORKDIR /service
CMD sleep 1 && ./runnableService "$(hostname -i)"

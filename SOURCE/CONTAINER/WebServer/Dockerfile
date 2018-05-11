FROM alpine:latest
RUN mkdir /service
COPY runnableService /service
COPY index.html /service
WORKDIR /service
CMD sleep 1 && ./runnableService "$(hostname -i)"

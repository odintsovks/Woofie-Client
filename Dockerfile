FROM alpine:latest AS base

RUN apk add --no-cache --update cmake g++ gcc make qt6-qtbase qt6-qtbase-dev qt6-qtbase-x11 libgcc libstdc++

COPY Woofie /src

RUN cmake -S src
RUN cmake --build .

ARG UID=10001
RUN adduser \
    --disabled-password \
    --gecos "" \
    --home "/nonexistent" \
    --shell "/sbin/nologin" \
    --no-create-home \
    --uid "${UID}" \
    appuser
USER appuser

ENTRYPOINT [ "/Woofie" ]

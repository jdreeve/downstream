FROM coinor/coin-or-optimization-suite

USER root

WORKDIR /var/coin-or/Cbc

RUN ./configure

RUN make

RUN make test

RUN make install


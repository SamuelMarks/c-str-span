FROM alpine

RUN apk add --no-cache gcc cmake git make musl-dev linux-headers

COPY . /c_str_span

WORKDIR /c_str_span

RUN cmake -DCMAKE_BUILD_TYPE='Debug' \
          -DBUILD_TESTING=1 \
          -DC_STR_SPAN_BUILD_TESTING=1 \
          -S . -B build && \
    cmake --build build

CMD cd build && ctest .

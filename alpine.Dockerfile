FROM alpine

RUN apk add --no-cache gcc cmake git make musl-dev linux-headers

COPY . /c_str_span

WORKDIR /c_str_span

RUN cmake -DCMAKE_BUILD_TYPE='Debug' \
          -DBUILD_TESTING=1 \
          -DBUILD_TESTING_c_str_span=1 \
          -S . -B build && \
    cmake --build build

CMD cd build && ctest .

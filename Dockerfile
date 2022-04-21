FROM alpine

RUN apk add --no-cache gcc cmake make musl-dev linux-headers

COPY . /c_str_span

WORKDIR /c_str_span/build

RUN cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    cmake --build .

CMD ctest .

# Build stage
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source code and CMakeLists
COPY CMakeLists.txt .
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/
COPY benchmarks/ ./benchmarks/

# Build the project
RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build . --target routing_engine

# Run stage (lightweight)
FROM ubuntu:22.04

WORKDIR /app

# Copy the compiled binary from the builder stage
COPY --from=builder /app/build/routing_engine .

# Expose the HTTP Server port
EXPOSE 8080

# Run the engine
CMD ["./routing_engine"]

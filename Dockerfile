# Use Debian as the base image
FROM debian:bullseye

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the project files
COPY . .

# Remove any existing build directory and CMake cache
RUN rm -rf build CMakeCache.txt

# Create build directory and build the project
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

# Run the example
CMD ["./build/CoreUtilsExample"]
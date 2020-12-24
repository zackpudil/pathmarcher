FROM nvidia/opengl:1.0-glvnd-runtime-ubuntu16.04
RUN apt-get update && apt-get install -y --no-install-recommends \
        mesa-utils \
        ocl-icd-libopencl1 \
        g++ \
        cmake \
        clinfo && \
    rm -rf /var/lib/apt/lists/*
RUN mkdir -p /etc/OpenCL/vendors && \
    echo "libnvidia-opencl.so.1" > /etc/OpenCL/vendors/nvidia.icd
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility, display
# This dockerfile contains commands for a base environment suitable for
# building manylinux pip wheels.
# The default gcc/g++ is 9.3
# Conda is used to manage Python versions.
#
# This dockerfile installs roadrunner dependencies and llvm package
# But does not build roadrunner, which is the subject of another dockerfile
#
# When looking for bugs in the many linux build:
#   1) Ensure you have the docker dependencies image:
#       - docker pull ciaranwelsh/roadrunner-manylinux2014-base:latest
#   2) Change git branch to the one you want to build (the one where your tests are failing):
#       - RUN cd $roadrunnerSource && git pull && git checkout expand-unittests
#   3) Run docker build . inside the "docker/roadrunner-manylinux2014-build" folder
#
#
######################################################
#  Commands to get docker working with clion
#
# Build:
#  docker build -t ciaranwelsh/roadrunner-manylinux2014-build:latest -f Dockerfile .
# Once built, run with
#  docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name clion_remote_env clion/remote-cpp-env:0.5
# Where:
#   -d runs the container as a daemon, so control returns back to you.
#   --cap-add sys_ptrace adds the ptrace capability, which is necessary for debugging. Docker containers are usually very minimal, by default, so you sometimes need to enable fundamental things.
#   -p2222:22 specifies a port mapping. Port 22 (the default SSH port) inside the container is mapped to outside the container as port 2222 on the host environment.
#        There’s nothing special about 2222. Doubling up, or multiplying by 10 are common ways of expressing mapped ports,
#       but you could specify any available port number, here. That can be useful if you intend to run multiple containers side-by-side (e.g. to support alternate toolchains).
#       Whatever port number you chose, since it’s only temporarily mapped, and may be reused, it’s usually worth clearing any cached SSH keys:
#

# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker build -t clion/centos7-cpp-env:0.1 -f Dockerfile.centos7-cpp-env .
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 clion/centos7-cpp-env:0.1
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"
#
# stop:
#   docker stop clion_remote_env
#
# ssh credentials (test user):
#   user@password

FROM ciaranwelsh/roadrunner-manylinux2014-base

ENV conda /Miniconda3/bin/conda
ENV bashrc /root/.bashrc
ENV roadrunnerSource /roadrunner
ENV roadrunnerBuild37 /roadrunner/build37
ENV roadrunnerBuild38 /roadrunner/build38
ENV roadrunnerBuild39 /roadrunner/build39
ENV roadrunnerInstall37 /roadrunner/install37
ENV roadrunnerInstall38 /roadrunner/install38
ENV roadrunnerInstall39 /roadrunner/install39
ENV swigExec /swig-4.0.2/install-swig-4.0.2/bin/swig

# install numpy in all conda envs
RUN $conda init && . $bashrc \
    && conda activate py37 \
    && pip install numpy \
    && conda activate py38 \
    && pip install numpy \
    && conda activate py39 \
    && pip install numpy

# install a dependency
RUN yum install -y ncurses-devel openssh-server

RUN git clone https://github.com/sys-bio/roadrunner.git $roadrunnerSource
RUN pwd
RUN cd $roadrunnerSource && git pull && git checkout cvodes-2

## build against python 3.7
#RUN $conda init && . $bashrc && conda activate py37 && cd $roadrunnerSource \
#    && mkdir -p $roadrunnerBuild37 && cd $roadrunnerBuild37 \
#    && cmake -DLLVM_INSTALL_PREFIX=/llvm-6.x/install-llvm-6.x -DRR_DEPENDENCIES_INSTALL_PREFIX=/libroadrunner-deps/install-libroadrunner-deps -DCMAKE_INSTALL_PREFIX=$roadrunnerInstall37 -DBUILD_PYTHON=ON -DBUILD_RR_PLUGINS=ON -DBUILD_TESTS=ON -DPython_ROOT_DIR=/Miniconda3/envs/py37 -DSWIG_EXECUTABLE=$swigExec ..
#RUN cd $roadrunnerBuild37 \
#    && cmake --build . --target install --config Release -j 1



#RUN $conda init && . $bashrc && cd $roadrunnerBuild37 \
#    && ctest --verbose --extra-verbose
#
## build against python 3.8
#RUN $conda init && . $bashrc && conda activate py38 && cd $roadrunnerSource \
#    && mkdir -p $roadrunnerBuild38 && cd $roadrunnerBuild38 \
#    && cmake -DLLVM_INSTALL_PREFIX=/llvm-project/llvm/install-llvm-6.x-rel -DRR_DEPENDENCIES_INSTALL_PREFIX=/libroadrunner-deps/install-libroadrunner-deps -DCMAKE_INSTALL_PREFIX=$roadrunnerInstall38 -DBUILD_PYTHON=ON -DBUILD_RR_PLUGINS=ON -DBUILD_TESTS=ON -DPython_ROOT_DIR=/Miniconda3/envs/py38 ..
#RUN cd $roadrunnerBuild38 \
#    && cmake --build . --target install --config Release -j 12
#RUN $conda init && . $bashrc && cd $roadrunnerBuild38 \
#    && ctest --verbose --extra-verbose
#
#
## build against python 3.9
#RUN $conda init && . $bashrc && conda activate py39 && cd $roadrunnerSource \
#    && mkdir -p $roadrunnerBuild39 && cd $roadrunnerBuild39 \
#    && cmake -DLLVM_INSTALL_PREFIX=/llvm-project/llvm/install-llvm-6.x-rel -DRR_DEPENDENCIES_INSTALL_PREFIX=/libroadrunner-deps/install-libroadrunner-deps -DCMAKE_INSTALL_PREFIX=$roadrunnerInstall39 -DBUILD_PYTHON=ON -DBUILD_RR_PLUGINS=ON -DBUILD_TESTS=ON -DPython_ROOT_DIR=/Miniconda3/envs/py39 ..
#RUN cd $roadrunnerBuild39 \
#    && cmake --build . --target install --config Release -j 12
#RUN $conda init && . $bashrc && cd $roadrunnerBuild39 \
#    && ctest --verbose --extra-verbose
#
#
# CMake location : /usr/local/bin/cmake
# make location : /usr/bin/make
# roadrunner dependency pckage: /libroadrunner-deps
# C++ compiler: /opt/rh/devtoolset-9/root/usr/bin/g++
# C compiler: /opt/rh/devtoolset-9/root/usr/bin/gcc
# -DCMAKE_INSTALL_PREFIX=../install-manylinux2014-rel
# -DLLVM_INSTALL_PREFIX=/llvm-6.x/install-llvm-6.x
# -DRR_DEPENDENCIES_INSTALL_PREFIX=/libroadrunner-deps/install-libroadrunner-deps
# -DBUILD_PYTHON=ON
# -DBUILD_TESTS=ON
# -DSWIG_EXECUTABLE=/swig-4.0.2/swig
# -DPython_ROOT_DIR=/Miniconda3/envs/py39
# -DBUILD_RR_PLUGINS=ON

#RUN ssh-keygen -A
#RUN ( \
#    echo 'LogLevel DEBUG2'; \
#    echo 'PermitRootLogin yes'; \
#    echo 'PasswordAuthentication yes'; \
#    echo 'Subsystem sftp /usr/libexec/openssh/sftp-server'; \
#  ) > /etc/ssh/sshd_config_test_clion
#
#RUN useradd -m user \
#  && yes password | passwd user
#RUN pwd
#
#CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]







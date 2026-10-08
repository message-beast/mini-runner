#!/bin/bash
#create a setup for mrn
sudo mkdir -p /usr/lib/mrn
sudo mkdir -p /usr/lib/mrn/data
sudo mkdir -p /usr/lib/mrn/backup
sudo mkdir -p /usr/lib/mrn/bin
#clone the repository
git clone https://github.com/message-beast/mini-runner
cd mini-runner/sycalls/x86/64-bit
make compile-close
make compile-fstatat
make compile-unlinkat
make compile-rmdir
make compile-getdents64
make compile-openat
#generate a header file for describing your cpu
cd ../../../compiler_guide
make compile
./compiler_guide
cd ../
#compile mrn binary
sudo make compile
echo 'export PATH="export PATH=$PATH:/usr/lib/mrn/bin"' >> ~/.bashrc
cat << 'EOF' >> ~/.bashrc
alias sudo='sudo env "PATH=$PATH"'
EOF
source ~/.bashrc
#installation has been finished time to try it out
sudo mrn --help
#install mrn-run (mrn_interpreter/bash alternative)
cd mrn_interpreter
make compile
cd ../
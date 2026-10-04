# MRN (MINI-RUNNER/minimal-resource-usage-runner)
mrn is a free software built up on a linux kernel to manage deployments and use the linux as the platform and it involved a lot of things and mini is for resource usage not for what it does.

# What is MRN?
MRN is a free software that it just turns any linux to paas and it is not a paas. it is a paas like program that you install on your vps or your machines to turn them into production grade servers. for example when renting VPS you can use mrn rather than docker or systemd or anything. mrn details are listed below.

### REQUIREMENTS
#### Kernel version >= 2.6.24
#### gcc
#### at least 8kb of memory
#### thiny core (intel/amd/arm either 32 bit or 64 bit)

## Manual installation
```bash
    #create a setup for mrn
    sudo mkdir -p /usr/lib/mrn
    sudo mkdir -p /usr/lib/mrn/data
    sudo mkdir -p /usr/lib/mrn/backup
    sudo mkdir -p /usr/lib/mrn/bin
    #clone the repository
    git clone https://github.com/message-beast/mini-runner
    # make sure you are using x86_64 other wise choose other folder <arch type> / <processor width>
    cd mini-runner/sycalls/x86/64-bit
    make compile-close
    make compile-fstatat
    make compile-unlinkat
    make compile-rmdir
    make compile-getdents64
    make compile-openat
    cd ../../../
    sudo make compile
    echo 'export PATH="export PATH=$PATH:/usr/lib/mrn/bin"' >> ~/.bashrc
    echo 'alias sudo="sudo env \"PATH=\$PATH\""' >> ~/.bashrc
    source ~/.bashrc
    #installation has been finished time to try it out
    sudo mrn --help
```
if this doesn't work for you see [manual compilation guide](https://github.com/message-beast/mini-runner/blob/master/Documentation/compile-guide.txt)


### MRN CAPABILITIES
mrn is powerful software that just removes unnecessary overhead and it is simple to use and gives much control to the user. the things that mrn has:
#### -> services management
#### -> jobs management
#### -> resource control
#### -> resource monitoring
#### -> time-shift
#### -> environment variable isolation

### MRN DESIGN PHILOSOPHY
#### -> mrn is created for developers and devops who uses linux -> linux mechanisms but if you are not using any os dependencies you can use mrn in deployment.
### no single point of failure
-- means that the services are not being supervised. They are only launch if you launch or kill if you killed. You're the manager MRN can't decide to kill your program on random Tuesday or restart it. if you wanna get to how MRN solves the point of failure and to be adapted to MRN environment it is better to see how you can use mrn. [More info](https://github.com/message-beast/mini-runner/blob/master/Documentation/usage.txt)
### services are integrated to github
-- means that the service control decision and their cloning directories are determined by MRN and also if you wanna update mrn must know about it. so services are basically integrated to github but if you have a problem updating like network failures or something you are going to handle it. mrn doesn't handle it. it is not its job
### jobs are repeatedly running finite processes
-- means that jobs must always be finite and run in a time what you estimate you basically tells mrn to add a job to your mrn so it can run it in some time interval. you can use jobs for example for database backups or something you wanna run based on some time interval
### jobs are given to you
-- means that mrn doesn't care where your code is from github or from your vps it only cares about where it is found and it does even delete that folder if you even delete that directory jobs and services are handled differently in MRN philosophy
### always state is snapshoted
-- means that mrn do automatic snapshot of the configuration that it is going to apply the changes you make. so you don't lose anything by exiting the program on sudden shutdown. for example adding services or by doing ctrl + c it always handles the signals to backup to normal state
### MRN Time shift
-- means that you can manually create a snap shoot of the internal files if you are not sure that the machine can't go down while making mrn changes so you just create it ,manually and apply it manually if something strong forces mrn to be killed. check out the docs for more info
### MRN-RUN alternative to bash
-- means that MRN does have its own execution program you can use it is not implemented by default and the interpreter works 100% fine but the services supports bash by default. MRN has mrn-run interpreter because having bash can slow down your services/jobs startup so in mrn-run you just type the commands that you run in normal bash and you write it down line by line so mrn executes it line by line and mrn-run doesn't support any of bash languages features it is more tend to be running a binary with its arguments line by line
### MRN should be compiled in user machine
-- this is a design choice that instead of just distributing binary we ship the source and you just run installation binary that is going to work. and this is chosen because if you even run infrastructure on mrn it can setup millions of services in seconds and for that for specific calculations we use special cpu features that is going to be selected at the installation time and creates a header file for describing your cpu
### MRN ARCH
-- mrn is currently works on x86 and arm 64 bit and 32 bit systems and support both cgroup v1 and v2
### MRN auto restart
-- this feature is being developed and it just be the most important one to auto restart both jobs and services for those whom were running
### MRN runs as a sudo
-- MRN follows a specific kind of execution state that it loads states before your programs begins and apply it if your program makes a change after mrn main thread completed. so the internal mrn files lives in /usr/lib/mrn that you are going to be forced to use sudo for mrn and it makes sense for a deployment
### No helmet
-- MRN doesn't stop you from running wrong binaries or harmful things it is just does its job if you don't know what you are running the problem is yours not mrn's
### MRN is independent of systemd
-- means that it is not systemd service. it handles the state when the machine turns on by its own and launches systemd before it. so you can use the stetted up environment for many cases. you need that for example NetworkManager is a systemd service. systemd must be launched before your programs and if you doesn't use systemd you can configure the entry program to use another binary that runs as the program mrn launches. if mrn doesn't find anything to run it is just runs your programs assuming that you don't want any user space things.
## MRN CURRENT STATUS
mrn is still in development and its work is 98% done. what remains is to write installation program and testing for arm processors. if you wanna compile and configure manually please follow [docs for compiling MRN](https://github.com/message-beast/mini-runner/blob/master/Documentation/compile-guide.txt)
## Getting started
if you already installed MRN it is better to run --help message or check out the [Documentation](https://github.com/message-beast/mini-runner/blob/master/Documentation/usage.txt)

### Author:
Name: Message Beast (Melikt Belay)
#### Message: 
I created mrn because paas are too expensive and waste half of the resource you used on them. you pay for their stuff than what you actually used and i heared something called VPS so i just wanna create a program that is going to manage the machine and gives me like paas environment.

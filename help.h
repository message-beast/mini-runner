#define HELP_SERVICE "mrn add <github repo> -- this adds the github repo to your process and then update them when the update is going!\n\tmrn remove <service name> -- this removes the service\n\tmrn stop -- this stops all running processes of your miroservices and macroservices\n\tmrn run <bash/mrn to run> --this just runs your bash on linux vps to start some services or servers\n\tmrn update <service> --this updates and pull github requests if it fails you just do the merge your self\n\t\tyou can use --stop to stop running the service from running while updating or just use the default which has do down time\n\tmrn stop <service name> -- this stops the service by its name\n\tmrn list -- this lists all the services\n\tmrn find <service name> -- this finds service by its name pattern matching, it is developed if you incase forget the name of your service\n\t\t --sort this sorts the results depending on their name matching score\n\tmrn run <service name> -- this runs your service but you can specify the running file after it\n\t\t --example: mrn run myservice run.sh\n\t\t\t    mrn run myservice run.sh --attach (attach is if you are looking to interact your service)\n\tmrn rename <old service name> <new service name> -- this changes the name of your service also renames the service program directory\n\tmrn neglect-updates --this just ignores if the updates avialable or not  and back to normal state to run your services\n\tmrn restart <service name> -- this restarts the services that is running/not\n\t\t--example mrn restart myservice\n\t\t\t  mrn restart myservice --attach (attach is just if you wanna test it interacting with it)\n\tmrn rename <old service name> <new name> -- this renames all the settings to your new name including cgroups\n\tmrn set-limit <service name> -cpu <number of cores> -mem <amount>\n\t\tfor above 4gb you must use --type supported types are:\n\t\tstandard -- the default\n\t\tcustom -- this is for gbs\n\t\textrene -- this is for teara and above\n\t\t\texample: mrn set-limit myservice -cpu 2.4 -mem 2g -- this sets the resource limit for service \"my service\" about 2.4 cores and 2gb of memory\n\tmrn res-usage <service name> -- this shows the resource using while typing that by the service\n\tmrn res-limit <service name> this shows the resource limits of the service\n"



#define JOB_HELP "mrn add-job <job name> <runnable file> <interval time> -- this adds a job to mrn\n\tmrn remove-job <job name> --this removes a job from job lists\n\tmrn warmup-jobs -- you don't have to run this mrn does that for you automatically when the device turned on\n\tmrn config job --max-cpu <number of cores> | -mc <number of cores> --max-memory <amount> | -mm <amount> -- this sets the job runner how much resource to use\n\tmrn show-job-limit -- this shows the mrn job configuration\n\tmrn restart-jobs -- this restarts the job runner if your jobs weren't running\n\tmrn stop-jobs -- if you wanna to kill the job runner you can do this\n\tmrn job-status -- this tells you weather the job runner is running or not\n\tmrn list-jobs --this lists all jobs and details\n"



#define TIME_HELP "mrn time-shift create --this creates and updates the current time-shift to be save as the current state\n\tmrn time-shift apply -- this applies the time shift you have"



#define HELP_ENV "mrn add-env <service name> <variable> <value> -- this adds the variable and its value to the service\n\tmrn show-env <service name> <variable> -- this shows the environment variable, value and where it is avialable\n\tmrn remove-env <service name> <variable> -- this removes the environment variable from service\n\tmrn update-env <service name> <variable> <new value> -- this updates the service environment variable to new value\n"



#define ABOUT "MRN is a well written c program that supports services, isolation, resource controll, signal managment, environment isolation, time shift, jobs, mrn-run (mrn interpreted language | alternative to bash)\n"



#define HELP_FUN\
    static inline __attribute((always_inline)) void displayHelp() {\
    printf("\033[32m Welcome to mrn (Mini-Runner/Minimal-Resource-Usage Runner) built to take any raw linux to paas tier managment and supports both 32bit and 64 bt systems with intel, amd and arm processors and supports both cgroup v1 and cgroup v2,which also gives the full paas program you need with a full controll over the machine\n\n\033[33m Developed by Melikt Belay!\n\n\033[0mHere is service uses cases you can make:\n\n\t%s\n\033[33m major use cases for jobs:\n\n\t\033[0m%s\n\033[33mmajor use case for time-shift:\n\n\t\033[0m%s\n\033[33mmajor use cases for environment variables\n\n\t\033[0m%s\n\033[32mAbout MRN\n\t\033[0m%s", HELP_SERVICE, JOB_HELP, TIME_HELP, HELP_ENV, ABOUT);\
}\


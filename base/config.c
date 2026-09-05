int numberOfProjects = 0;
int numberOfJobs = 0;
int numberOfJobsDaemon = 0;
#define __INITIAL_SCALE_SIZE_OF_SERVICES__ 4
int capacityOfServices = __INITIAL_SCALE_SIZE_OF_SERVICES__;
#define __MAX_CORE_VIOLATION 3
#define __ERROR_FORMATING_STR 4
#define __INITIAL_SCALE_SIZE_OF_JOBS__ 4
int capacityOfJobs = __INITIAL_SCALE_SIZE_OF_JOBS__;
int capacityOfJobsDaemon = __INITIAL_SCALE_SIZE_OF_JOBS__;
#define __DEFAULT_TIME_EXPECTATION_SECONDS_SCALE__FOR_JOBS__ 20
#define __INITIAL_SCALE_NUM_ARGS__ 20
#define __INITIAL_SCALE_OF_ENV__ 20
int numberOfEnv = 0;
int capacityOfEnv = __INITIAL_SCALE_OF_ENV__;
//#define DEBUG_MODE 1
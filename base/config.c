__attribute__((aligned(64))) int numberOfProjects = 0;
__attribute__((aligned(64))) int numberOfJobs = 0;
__attribute__((aligned(64)))int numberOfJobsDaemon = 0;
#define __INITIAL_SCALE_SIZE_OF_SERVICES__ 4
__attribute__((aligned(64))) int capacityOfServices = __INITIAL_SCALE_SIZE_OF_SERVICES__;
__attribute__((aligned(64))) int numberOfCloneProjects = 0;
__attribute__((aligned(64))) int capacityOfCloneServices = __INITIAL_SCALE_SIZE_OF_SERVICES__;
#define __MAX_CORE_VIOLATION 3
#define __ERROR_FORMATING_STR 4
#define __INITIAL_SCALE_SIZE_OF_JOBS__ 4
__attribute__((aligned(64))) int capacityOfJobs = __INITIAL_SCALE_SIZE_OF_JOBS__;
__attribute__((aligned(64)))int capacityOfJobsDaemon = __INITIAL_SCALE_SIZE_OF_JOBS__;
#define __DEFAULT_TIME_EXPECTATION_SECONDS_SCALE__FOR_JOBS__ 20
#define __INITIAL_SCALE_NUM_ARGS__ 20
#define __INITIAL_SCALE_OF_ENV__ 20
__attribute__((aligned(64))) int numberOfEnv = 0;
__attribute__((aligned(64)))int capacityOfEnv = __INITIAL_SCALE_OF_ENV__;
#define __INITIAL_SCALE_OF_ENV_GROUP__ 10
__attribute__((aligned(64))) int numberOfResLimits = 0;
#define __INITIAL_SCALE_OF_RES_LIMIT__ 4
__attribute__((aligned(64))) int capacityOfResLimits = __INITIAL_SCALE_OF_RES_LIMIT__;
//#define DEBUG_MODE 1
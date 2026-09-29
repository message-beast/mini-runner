#pragma once
extern int numberOfProjects;
extern int numberOfJobs;
extern int numberOfJobsDaemon;
extern int numberOfCloneProjects;
extern int capacityOfCloneServices;
#define __INITIAL_SCALE_SIZE_OF_SERVICES__ 4
extern int capacityOfServices;
#define __MAX_CORE_VIOLATION 3
#define __ERROR_FORMATING_STR 4
#define __INITIAL_SCALE_SIZE_OF_JOBS__ 4
extern int capacityOfJobs;
extern int capacityOfJobsDaemon;
#define __DEFAULT_TIME_EXPECTATION_SECONDS_SCALE__FOR_JOBS__ 20
#define __INITIAL_SCALE_NUM_ARGS__ 20
#define __INITIAL_SCALE_OF_ENV__ 20
extern int numberOfEnv;
extern int capacityOfEnv;
#define __INITIAL_SCALE_OF_ENV_GROUP__ 10
#define __INITIAL_SCALE_OF_RES_LIMIT__ 4
extern int numberOfResLimits;
extern int capacityOfResLimits;


/*internal files definition*/

#define __FILE_ENV "/usr/lib/mrn/data/env"
#define __FILE_ENV_COPY "/usr/lib/mrn/data/env.backup"
#define __FILE_JOB_CPU_LIMIT "/usr/lib/mrn/data/job_cpu_limit"
#define __FILE_JOB_CPU_LIMIT_COPY "/usr/lib/mrn/data/job_cpu_limit.backup"
#define __FILE_JOB_DAEMON_PID "/usr/lib/mrn/data/job_daemon_pid"
#define __FILE_JOB_DAEMON_PID_COPY "/usr/lib/mrn/data/job_daemon.backup"
#define __FILE_JOB_MEM_LIMIT "/usr/lib/mrn/data/job_mem_limit"
#define __FILE_JOB_MEM_LIMIT_COPY "/usr/lib/mrn/data/job_mem_limit.backup"
#define __FILE_JOBS "/usr/lib/mrn/data/jobs"
#define __FILE_JOBS_COPY "/usr/lib/mrn/data/jobs.backup"
#define __FILE_JOBS_SYNC "/usr/lib/mrn/data/jobs_sync"
#define __FILE_JOBS_SYNC_COPY "/usr/lib/mrn/data/jobs_sync.backup"
#define __FILE_LIMITS "/usr/lib/mrn/data/limits"
#define __FILE_LIMITS_COPY "/usr/lib/mrn/data/limits.backup"
#define __FILE_PROJECTS "/usr/lib/mrn/data/projects"
#define __FILE_PROJECTS_COPY "/usr/lib/mrn/data/projects.backup"
#define __FILE_UPDATE_STATUS "/usr/lib/mrn/data/updateStatus"
#define __FILE_UPDATE_STATUS_COPY "/usr/lib/mrn/data/updateStatus.backup"





#define __FILE_ENV_BACKUP "/usr/lib/mrn/backup/env"
#define __FILE_ENV_COPY_BACKUP "/usr/lib/mrn/backup/env.backup"
#define __FILE_JOB_CPU_LIMIT_BACKUP "/usr/lib/mrn/backup/job_cpu_limit"
#define __FILE_JOB_CPU_LIMIT_COPY_BACKUP "/usr/lib/mrn/backup/job_cpu_limit.backup"
#define __FILE_JOB_DAEMON_PID_BACKUP "/usr/lib/mrn/backup/job_daemon_pid"
#define __FILE_JOB_DAEMON_PID_COPY_BACKUP "/usr/lib/mrn/backup/job_daemon.backup"
#define __FILE_JOB_MEM_LIMIT_BACKUP "/usr/lib/mrn/backup/job_mem_limit"
#define __FILE_JOB_MEM_LIMIT_COPY_BACKUP "/usr/lib/mrn/backup/job_mem_limit.backup"
#define __FILE_JOBS_BACKUP "/usr/lib/mrn/backup/jobs"
#define __FILE_JOBS_COPY_BACKUP "/usr/lib/mrn/backup/jobs.backup"
#define __FILE_JOBS_SYNC_BACKUP "/usr/lib/mrn/backup/jobs_sync"
#define __FILE_JOBS_SYNC_COPY_BACKUP "/usr/lib/mrn/backup/jobs_sync.backup"
#define __FILE_LIMITS_BACKUP "/usr/lib/mrn/backup/limits"
#define __FILE_LIMITS_COPY_BACKUP "/usr/lib/mrn/backup/limits.backup"
#define __FILE_PROJECTS_BACKUP "/usr/lib/mrn/backup/projects"
#define __FILE_PROJECTS_COPY_BACKUP "/usr/lib/mrn/backup/projects.backup"
#define __FILE_UPDATE_STATUS_BACKUP "/usr/lib/mrn/backup/updateStatus"
#define __FILE_UPDATE_STATUS_COPY_BACKUP "/usr/lib/mrn/backup/updateStatus.backup"



//#define DEBUG_MODE 1


#define __MRN_VERSION__ "1.0.0"
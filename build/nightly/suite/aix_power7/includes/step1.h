# QSUB -p %USER_PRIORITY:70%
# @ ec_smt=%EC_SMT:yes%

#=======================================================================
# step1.h
# Define second step for copying the logfile when on the ibm-cluster 
#=======================================================================
# @ step_name=step_1
# @ queue
# @ dependency=(step_1==0 || step_1!=0)
# @ class=%QUEUE_EPILOG:ns%
# @ user_priority=60
# @ output = /dev/null
# @ error =  /dev/null
# @ job_name = $(jobid)
# @ job_type=serial

# @ step_name = step_2
# @ node_usage=shared
# @ resources = ConsumableCpus(1) ConsumableMemory(300mb)

# @ parallel_threads=1 
# requested originally on hpc as a fix for ll, removed for ecgate

export LOADL_STEP_NAME=${LOADL_STEP_NAME:-step_1}
#case $LOADL_STEP_NAME in
#step_1)

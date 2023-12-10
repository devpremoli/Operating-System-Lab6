// C program for implementation of Simulation 
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include "process.h"
#include "util.h"


typedef struct {
    int *array;
    int front, rear, size;
    unsigned capacity;
} Queue;

Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

int isFull(Queue* queue) { return (queue->size == queue->capacity); }
int isEmpty(Queue* queue) { return (queue->size == 0); }
void enqueue(Queue* queue, int item) {
    if (isFull(queue)) return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

int dequeue(Queue* queue) {
    if (isEmpty(queue)) return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

int isInQueue(Queue* queue, int processIndex) {
    for (int i = queue->front; i != queue->rear; i = (i + 1) % queue->capacity) {
        if (queue->array[i] == processIndex) {
            return 1; // Process is in the queue
        }
    }
    // Check the last element in the queue
    if (queue->array[queue->rear] == processIndex) {
        return 1;
    }
    return 0; // Process is not in the queue
}

// Function to find the waiting time for all  
// processes
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) {
    int *rem_bt = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
    }

    Queue* queue = createQueue(n);
    int t = 0; // Current time
    int processed = 0; // Processed flag

    // Initially enqueue processes that have arrived
    for (int i = 0; i < n; i++) {
        if (plist[i].art <= t) {
            enqueue(queue, i);
            processed = 1;
        }
    }

    while (processed) {
        if (!isEmpty(queue)) {
            int i = dequeue(queue);
            processed = 0; // Reset flag

            // Calculate the time quantum for current process
            int slice = quantum < rem_bt[i] ? quantum : rem_bt[i];
            t += slice;
            rem_bt[i] -= slice;

            // Update waiting time for all other processes in the queue
            for (int j = 0; j < n; j++) {
                if (j != i && plist[j].art <= t && rem_bt[j] > 0) {
                    plist[j].wt += slice;
                }
            }

            if (rem_bt[i] > 0) {
                enqueue(queue, i); // Re-enqueue if not done
            }

            // Enqueue new arrivals
            for (int j = 0; j < n; j++) {
                if (plist[j].art <= t && rem_bt[j] > 0 && j != i) {
                    if (!isInQueue(queue, j)) { // Check if not already in queue
                        enqueue(queue, j);
                    }
                    processed = 1; // Set flag if at least one process is in queue
                }
            }
        } else {
            // No process in the queue, increment time
            t++;
            for (int j = 0; j < n; j++) {
                if (plist[j].art <= t && rem_bt[j] > 0) {
                    enqueue(queue, j);
                    processed = 1;
                }
            }
        }
    }

    free(rem_bt);
    free(queue->array);
    free(queue);
}



// Function to find the waiting time for all  
// processes 
void findWaitingTimeSJF(ProcessType plist[], int n)
{
  /*
       * 
         - Find process with minimum remaining time at every single time lap.
         - Reduce its time by 1.
         - Check if its remaining time becomes 0 
         - Increment the counter of process completion.
         - Completion time of *current process = current_time +1;*
         - Calculate waiting time for each completed process. *wt[i]= Completion time - arrival_time-burst_time*
         - Increment time lap by one.
 
     */
  int allProcesses = 0;
  int count = 0;
  int *completion_time = malloc(n * sizeof(int));
  int *temp = malloc(n * sizeof(int));

  for (int i = 0;  i < n; i++) {
    temp[i] = plist[i].bt;
    completion_time[i] = -1;
  }
	while (allProcesses < n) {
    int smallest = -1;
    int time = INT_MAX;
    for (int i = 0; i < n; i++) {
      if (temp[i] < time && completion_time[i] == -1 && plist[i].art <= count) {
        smallest = i;
        time = temp[i];
      }
    }
    
    temp[smallest] -= 1;
    if (temp[smallest] == 0) {
      allProcesses++;
      completion_time[smallest] = count + 1;
    }
    count++;
  }

  for (int i = 0; i < n;i++) {
    plist[i].wt = completion_time[i] - plist[i].art - plist[i].bt;
  }
  free(completion_time);
} 

// Function to find the waiting time for all  
// processes 
void findWaitingTime(ProcessType plist[], int n)
{ 
    // waiting time for first process is 0, or the arrival time if not 
    plist[0].wt = 0 +  plist[0].art; 
  
    // calculating waiting time 
    for (int  i = 1; i < n ; i++ ) 
        plist[i].wt =  plist[i-1].bt + plist[i-1].wt; 
} 
  
// Function to calculate turn around time 
void findTurnAroundTime( ProcessType plist[], int n)
{ 
    // calculating turnaround time by adding bt[i] + wt[i] 
    for (int  i = 0; i < n ; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 
  
// Function to sort the Process acc. to priority
int my_comparer(const void *this, const void *that)
{ 
  // * 1. Cast this and that into (ProcessType *)
  ProcessType *process1 = (ProcessType *) this;
  ProcessType *process2 = (ProcessType *) that;
  // * 2. return 1 if this->pri < that->pri
  if (process1->pri < process2->pri) {
    return 1;
  } else if (process1->pri > process2->pri) {
    return -1;
  }
  return 0;
} 

//Function to calculate average time 
void findavgTimeFCFS( ProcessType plist[], int n) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTime(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nFCFS\n");
}

//Function to calculate average time 
void findavgTimeSJF( ProcessType plist[], int n) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTimeSJF(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nSJF\n");
}

//Function to calculate average time 
void findavgTimeRR( ProcessType plist[], int n, int quantum) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTimeRR(plist, n, quantum); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

//Function to calculate average time 
void findavgTimePriority( ProcessType plist[], int n) 
{ 
  
   /*
    * 1- Sort the processes (i.e. plist[]), burst time and priority according to the priority.
    * 2- Now simply apply FCFS algorithm.
    */
    // for(int i=0;i<n;i++)
    // {
    //     int pos=i;
    //     int temp=plist[i].bt;
    //     plist[i].bt=plist[pos].bt;
    //     plist[pos].bt=temp;
    // }
    Comparer process_comparer = &my_comparer;
    qsort(plist,n,sizeof(ProcessType), process_comparer);
    // findavgTimeFCFS(plist,n);
    findWaitingTime(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
    //Display processes along with all details 
    printf("\n*********\nPriority\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
  
    // Calculate total waiting time and total turn  
    // around time 
    for (int  i=0; i<n; i++) 
    { 
        total_wt = total_wt + plist[i].wt; 
        total_tat = total_tat + plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

ProcessType * initProc(char *filename, int *n) 
{
  	FILE *input_file = fopen(filename, "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    ProcessType *plist = parse_file(input_file, n);
  
    fclose(input_file);
  
    return plist;
}
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;

    ProcessType *proc_list;
  
    if (argc < 2) {
		   fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
		   fflush(stdout);
		   return 1;
	   }
    
  // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
  
    findavgTimeFCFS(proc_list, n);
    
    printMetrics(proc_list, n);
  
  // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
   
    findavgTimeSJF(proc_list, n); 
   
    printMetrics(proc_list, n);
  
  // Priority
    n = 0; 
    proc_list = initProc(argv[1], &n);
    
    findavgTimePriority(proc_list, n); 
    
    printMetrics(proc_list, n);
    
  // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    
    findavgTimeRR(proc_list, n, quantum); 
    
    printMetrics(proc_list, n);
    
    return 0; 
}


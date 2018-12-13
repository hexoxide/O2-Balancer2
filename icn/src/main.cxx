#include <string.h>
#include <errno.h>
#include <string>
#include "zookeeper/zookeeper.h"
#include <unistd.h>
#include <stdarg.h>

static zhandle_t *zh;
int connected = 0;
int expired = 0;
static struct String_vector* epns = NULL;

/** main Watcher function , cotains events for connecting status */
void watcher(zhandle_t *zzh,
              int type,
              int state,
              const char *path,
              void *watcherCtx)
  {
    if (type == ZOO_SESSION_EVENT) {
       if (state == ZOO_CONNECTED_STATE) {
           connected = 1;

           printf("Received a connected event.\n");
       } else if (state == ZOO_CONNECTING_STATE) {
           if(connected == 1) {
               printf("Disconnected.\n");
           }
           connected = 0;
       } else if (state == ZOO_EXPIRED_SESSION_STATE) {
           expired = 1;
           connected = 0;
           zookeeper_close(zzh);
       }
    }
}
/*
 * Functions to deal with workers and tasks caches
 */

 /*
  * Auxiliary functions
  */

 char * make_path(int num, ...) {
     const char * tmp_string;

     va_list arguments;
     va_start ( arguments, num );

     int total_length = 0;
     int x;
     for ( x = 0; x < num; x++ ) {
         tmp_string = va_arg ( arguments, const char * );
         if(tmp_string != NULL) {
             //LOG_DEBUG(("Counting path with this path %s (%d)", tmp_string, num));
             total_length += strlen(tmp_string);
         }
     }

     va_end ( arguments );

     char * path = (char*)malloc(total_length * sizeof(char) + 1);
     path[0] = '\0';
     va_start ( arguments, num );

     for ( x = 0; x < num; x++ ) {
         tmp_string = va_arg ( arguments, const char * );
         if(tmp_string != NULL) {
             //LOG_DEBUG(("Counting path with this path %s",tmp_string));
             strcat(path, tmp_string);
         }
     }

     return path;
 }

int contains(const char * child, const struct String_vector* children) {
  int i;
  for(i = 0; i < children->count; i++) {
    if(!strcmp(child, children->data[i])) {
      return 1;
    }
  }

  return 0;
}

/*
 * Allocate String_vector, copied from zookeeper.jute.c
 */
int allocate_vector(struct String_vector *v, int32_t len) {
    if (!len) {
        v->count = 0;
        v->data = 0;
    } else {
        v->count = len;
        v->data = (char**)calloc(sizeof(*v->data), len);
    }
    return 0;
}
/*
 * Functions to free memory
 */
void free_vector(struct String_vector* vector) {
    int i;

    // Free each string
    for(i = 0; i < vector->count; i++) {
        free(vector->data[i]);
    }

    // Free data
    free(vector -> data);

    // Free the struct
    free(vector);
}

struct String_vector* make_copy( const struct String_vector* vector ) {
    struct String_vector* tmp_vector = (String_vector *) malloc(sizeof(struct String_vector));

    tmp_vector->data = (char**) malloc(vector->count * sizeof(const char *));
    tmp_vector->count = vector->count;

    int i;
    for( i = 0; i < vector->count; i++) {
        tmp_vector->data[i] = strdup(vector->data[i]);
    }

    return tmp_vector;
}

struct String_vector* added_and_set(const struct String_vector* current,
                                    struct String_vector** previous) {
    struct String_vector* diff = (String_vector*) malloc(sizeof(struct String_vector));

    int count = 0;
    int i;
    for(i = 0; i < current->count; i++) {
	if((*previous) == NULL){
		count++;
	}
        else if (!contains(current->data[i], (*previous))) {
            count++;
        }
    }

    //allocate_vector(diff, count);

    //int prev_count = count;
    count = 0;
    for(i = 0; i < current->count; i++) {
       // if (!contains(current->data[i], (* previous))) {
            //diff->data[count] = (char*)malloc(sizeof(char) * strlen(current->data[i]) + 1);
            //memcpy(diff->data[count++], current->data[i], strlen(current->data[i]));
       // }
    }

    //free_vector((struct String_vector*) *previous);
    //(*previous) = make_copy(current);

    return diff;

}
void get_task_data(const char *);
void get_task_data_completion(int rc, const char *value, int value_len,
                              const struct Stat *stat, const void *data) {
    //int worker_index;

    switch (rc) {
        case ZCONNECTIONLOSS:
        case ZOPERATIONTIMEOUT:
            get_task_data((const char *) data);

            break;

        case ZOK:
            printf("key: \t");
            printf("%s\n", (char *) data);
            printf("value \t");
            printf("%s\n", strndup(value, value_len));
            // LOG_DEBUG(("Choosing worker for task %s", (const char *) data));

            // if(epns != NULL) {
            //     /*
            //      * Choose worker
            //      */
            //     epns_index = (rand() % epns->count);
            //     // LOG_DEBUG(("Chosen worker %d %d", worker_index, workers->count));
            //
            //     /*
            //      * Assign task to worker
            //      */
            //     struct task_info *new_task;
            //     new_task = (struct task_info*) malloc(sizeof(struct task_info));
            //
            //     new_task->name = (char *) data;
            //     new_task->value = strndup(value, value_len);
            //     new_task->value_len = value_len;
            //
            //     const char * worker_string = workers->data[worker_index];
            //     new_task->worker = strdup(worker_string);
            //
            //     LOG_DEBUG(("Ready to assign it %d, %s",
            //                worker_index,
            //                workers->data[worker_index]));
            //     task_assignment(new_task);
            // }

            break;
        default:
            //LOG_ERROR(("Something went wrong when checking the master lock: %s",rc2string(rc)));
            break;
    }
}
void get_task_data(const char *task) {
    if(task == NULL) return;

    //LOG_DEBUG(("Task path: %s",task));
    char * tmp_task = strndup(task, 15);
    char * path = make_path(2, "/EPN/", tmp_task);
    //LOG_DEBUG(("Getting task data %s",tmp_task));

    zoo_aget(zh,
             path,
             0,
             get_task_data_completion,
             (const void *) tmp_task);
    free(path);
}
void assign_tasks(const struct String_vector *strings) {
    /*
     * for each epn receive ip and port
     */
    //LOG_DEBUG(("Task count: %d", strings->count));
    int i;
    for( i = 0; i < strings->count; i++) {
        //LOG_DEBUG(("Assigning task %s", char *) strings->data[i]));
        get_task_data( strings->data[i] );
    }
}

void get_epns();
void epn_watcher (zhandle_t *zh,
                    int type,
                    int state,
                    const char *path,
                    void *watcherCtx)
  {
    printf("epn watcher triggered %s\n", std::string(path).c_str());
    if( type == ZOO_CHILD_EVENT) {
        get_epns();
    }
    printf("Tasks watcher done\n");
}
/**
 *
 * Completion function invoked when the call to get
 * the list of tasks returns.
 *
 */
void epn_completion (int rc,
                       const struct String_vector *strings,
                       const void *data) {
    //struct String_vector *tmp_tasks;
    switch (rc) {
        case ZCONNECTIONLOSS:
        case ZOPERATIONTIMEOUT:
          {
            get_epns();
          }
          break;
        case ZOK:
          {
            printf("Assigning epns\n");
           // struct String_vector *tmp_tasks = added_and_set(strings, &epns);
            assign_tasks(strings);
            for(int i = 0; i < strings->count; i++) {
                printf("%s", strings->data[i]);
            }
            //free_vector(tmp_tasks);
          }
          break;
        default:
            printf("Something went wrong when checking tasks: %d", rc);

            break;
    }
}
//asynch retrieev epn and place watcher
void get_epns () {
  printf("Getting tasks\n");
    zoo_awget_children(zh,
                       "/EPN",
                       epn_watcher,
                       NULL,
                       epn_completion,
                       NULL);
}


int main(int argc, char* argv[]) {
  char buffer[512];
  printf("starting program\n");
  zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

  zh = zookeeper_init("localhost:2181", watcher, 10000, 0, 0, 0);
  if (!zh) {
    return errno;
  }

//  struct ACL CREATE_ONLY_ACL[] = {{ZOO_PERM_CREATE, ZOO_AUTH_IDS}};
//  struct ACL_vector CREATE_ONLY = {1, CREATE_ONLY_ACL};
  int rc = zoo_create(zh,"/xyz","value", 5, &ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE,
                      buffer, sizeof(buffer)-1);

  /** this operation will fail with a ZNOAUTH error */
  int buflen= sizeof(buffer);
  struct Stat stat;
  rc = zoo_get(zh, "/xyz0000000006", 0, buffer, &buflen, &stat);
  if (rc) {
    fprintf(stderr, "Error %d \n", rc);
  }
  if (rc == ZOK) {
    std::string log = std::string(buffer, static_cast<unsigned long>(512));
    printf("opgehaalde value: %s\n", log.c_str());
   }
  get_epns();
  while(true){
    sleep(1);
  }
  zookeeper_close(zh);
  return 0;
}

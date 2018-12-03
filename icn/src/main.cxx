#include <string.h>
#include <errno.h>
#include <string>
#include "zookeeper/zookeeper.h"
#include <unistd.h>

static zhandle_t *zh;
int connected = 0;
int expired = 0;

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
    switch (rc) {
        case ZCONNECTIONLOSS:
        case ZOPERATIONTIMEOUT:
            get_epns();

            break;

        case ZOK:
            printf("Assigning epns\n");


            //struct String_vector *tmp_tasks = added_and_set(strings, &tasks);
            for(int i = 0; i < strings->count; i++) {
                printf("%s", strings->data[i]);
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

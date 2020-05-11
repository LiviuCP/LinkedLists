#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "../../LinkedListsLib/linkedlist.h"
#include "../ManualListEntry/apputils.h"

static const size_t nrOfPriorities = 5;
static const size_t priorities[] = {6, 2, 7, 3, 4};

// need global variable as it is accessed by child process termination handler
static List* list = NULL;

void gracefulTerminationHandler(int signalNumber);
void setGracefulTerminationHandler(void);
void terminateChild(pid_t childPid);

int main()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Unable to fork a child process");
        exit(-1);
    }

    if (0 == pid)                                                                                               // child
    {
        setGracefulTerminationHandler();

        puts("[CHILD] Creating list...\n");
        list = createListFromPrioritiesArray(priorities, nrOfPriorities);
        sleep(1);

        if (list == NULL)
        {
            fputs("[CHILD] Unable to create list. Exiting prematurely...\n", stderr);
            _exit(-1);
        }
        else
        {
            puts("[CHILD] Done. The content of the unsorted list is:\n");
            printList(list);
            puts("");
            puts("[CHILD] Going to sleep\n");
        }

        while (1)
        {
            sleep(1);
            puts("[CHILD] Woke up!");
            puts("[CHILD] Unsorted list has already been printed.");
            puts("[CHILD] It will be sorted prior to exiting");
            puts("[CHILD] Going back to sleep\n");
        }
    }
    else                                                                                                        // parent
    {
        puts("[PARENT] Sleeping for a time...\n");
        sleep(5);

        terminateChild(pid);
    }

    return 0;
}

void gracefulTerminationHandler(int signalNumber)
{
    printf("[CHILD] Received signal: %i\n", signalNumber);

    if (list != NULL)
    {
        puts("[CHILD] Sorting list before gracefully terminating...");
        sortAscendingByPriority(list);
        sleep(1);
        puts("[CHILD] Done. Sorted list content: \n");
        printList(list);
        puts("");
        deleteList(list, deleteObject);
        list = NULL;
    }

    puts("[CHILD] Terminating now...");
    _exit(0);
}

void setGracefulTerminationHandler()
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = gracefulTerminationHandler;
    sigaction(SIGTERM, &action, NULL);
}

void terminateChild(pid_t childPid)
{
    // send termination signal only if the child hasn't exited prematurely
    int status;
    if (waitpid(childPid, &status, WNOHANG) == childPid && WIFEXITED(status))
    {
        printf("[PARENT] Child exited with code: %d\n", WEXITSTATUS(status));
        puts("[PARENT] Will now exit too\n");
    }
    else
    {
        puts("[PARENT] No child status received, assuming it's running.");
        puts("[PARENT] Sending termination signal to child...");

        if (-1 == kill(childPid, SIGTERM))
        {
            perror("[PARENT] Unable to kill child process");
            exit(-1);
        }

        puts("[PARENT] Done");

        wait(NULL);
        puts("[PARENT] Child terminated, will now exit\n");
    }
}

/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

cfe_daemon.c -- Unix socket based server for the C++/C front end daemon.

This file is provided as an example of using the front end as a library to
build a multi-threaded client/server C++/C compiler front end.

This daemon and its corresponding client are not (currently) production grade
and are intended for exposition only.

See cfe_daemon_common.h for documentation on the daemon protocol.
*/

/* Header files common to all files. */
#include "fe_common.h"

/* See if this code is needed at all. */
#if EDG_FRONT_END_DAEMON

/* Verify the configuration is compatible with the multi-threaded driver
   code. */
#if !MAKE_FRONT_END_CALLABLE
 #error -- The mutli-threaded C++/C-daemon requires MAKE_FRONT_END_CALLABLE
#endif /* !MAKE_FRONT_END_CALLABLE */
#if !CUSTOM_DEFAULT_OUTPUT_FILES
 #error -- The multi-threaded C++/C-daemon requires CUSTOM_DEFAULT_OUTPUT_FILES
#endif /* !CUSTOM_DEFAULT_OUTPUT_FILES */

/* Common daemon code. */
#include "cfe_daemon_common.h"


/* Unix headers used to implement the daemon. */
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

/* Conditionally open the "edg" namespace. */
BEGIN_EDG_NAMESPACE


/*
This struct represents the arguments required to start a new front end thread.
*/
typedef struct an_invocation_ticket {
  int communication_fd;
} an_invocation_ticket;


static void destroy_ticket(an_invocation_ticket *ticket)
{
  if (ticket != NULL) {
    if (ticket->communication_fd != -1) close(ticket->communication_fd);
    free(ticket);
  }
}

static pthread_cond_t
                any_work;
                        /* This condition is awaited upon for threads looking
                           for work (guarded by the next ticket lock). */
static an_invocation_ticket
                *next_ticket;
                        /* The next ticket to work on (guarded by the next
                           ticket lock). */
static pthread_mutex_t
                next_ticket_lock;
                        /* The work lock is acquired to update the any_work
                           condition and next ticket value. */

typedef a_string a_daemon_string;

static void append_message(const char *msg, size_t len, void *context)
{
  edg_string_append_view((a_string *)context, edg_string_view(msg, len));
}

static a_boolean read_into_string(a_socket_reader *reader, a_string *result)
{
  edg_string_clear(result);
  return socket_reader_read(reader, append_message, result);
}

#if DEBUG

static _Thread_local a_daemon_string
                trans_unit_tag;
                        /* An identifying tag passed from the client.  This is
                           used to allow the client to provide some useful
                           identifying tag to associate a specific client with
                           the server (daemon) thread. */

static _Thread_local uintptr_t
                trans_unit_tag_hash;
                        /* A hash of trans_unit_tag; this can be useful when
                           using debuggers that capture a replayable execution
                           snapshot to apply break points to a specific
                           translation unit (without the more significant
                           performance impact of string comparison). */


uintptr_t hash_trans_unit_tag(a_const_char *str)
/*
This function exists to allow computing the trans_unit_tag_hash in a debugger
with ease.  The function intentionally has external linkage to avoid complaints
about an unused function when CHECKING is FALSE.
*/
{
  a_string_view view = edg_string_view(str, strlen(str));
  return edg_string_view_hash(&view);
}  /* hash_trans_unit_tag */

#endif /* DEBUG */

static _Thread_local int
                fd_thread_output;
                        /* The default output file descriptor for the current
                           thread. */


static FILE* get_new_output_file_for_thread()
/*
Return a new output FILE for the current thread.
*/
{
  FILE *result;
  int  fd = dup(fd_thread_output);

  if (fd == -1) {
    fputs("The daemon has run out of available file descriptors.\n", stderr);
    exit(RC_CATASTROPHE);
  }  /* if */
  result = fdopen(fd, "w");
  /* Do not buffer output; this ensures output is immediately forwarded without
     explicit fflush calls. */
  setbuf(result, NULL);
  return result;
}  /* get_new_output_file_for_thread */


FILE* default_error_output_file()
/*
Return the default error file for the current thread.
*/
{
  return get_new_output_file_for_thread();
}  /* default_error_output_file */


FILE* default_preproc_output_file()
/*
Return the preprocessor output file for the current thread.
*/
{
  return get_new_output_file_for_thread();
}  /* default_preproc_output_file */

#if NEED_IL_DISPLAY

FILE* default_il_display_output_file()
/*
Return the default IL display output file for the current thread.
*/
{
  return get_new_output_file_for_thread();
}  /* default_il_display_output_file */

#endif /* NEED_IL_DISPLAY */

static void process_ticket(const an_invocation_ticket *ticket)
{
  a_socket_reader reader;
  a_string request, arguments, exit_message;
  Edg_array argv;
  edg_string_init(&request, EDG_ALLOC_DIRECT);
  edg_string_init(&arguments, EDG_ALLOC_DIRECT);
  edg_string_init(&exit_message, EDG_ALLOC_DIRECT);
  edg_array_init(&argv, sizeof(char *), EDG_ALLOC_DIRECT, 0, NULL, NULL);
  socket_reader_init(&reader, ticket->communication_fd);
  if (!read_into_string(&reader, &request)) goto client_read_failure;
  if (strcmp(request.data ? request.data : "", ping_request_bytes) == 0) goto done;
  if (strcmp(request.data ? request.data : "", exec_request_bytes) != 0) {
    fprintf(stderr, "Received bad request tag \"%s\" from client.\n", request.data ? request.data : "");
    goto done;
  }
#if DEBUG
  if (trans_unit_tag.cap == 0) edg_string_init(&trans_unit_tag, EDG_ALLOC_DIRECT);
  if (!read_into_string(&reader, &trans_unit_tag)) goto client_read_failure;
  trans_unit_tag_hash = hash_trans_unit_tag(trans_unit_tag.data ? trans_unit_tag.data : "");
#endif
  fd_thread_output = ticket->communication_fd;
  if (!read_into_string(&reader, &arguments)) goto client_read_failure;
  {
    int argc = atoi(arguments.data ? arguments.data : "");
    char *arg = arguments.data;
    for (int i = 0; i < argc; ++i) {
      arg += strlen(arg) + 1;
      edg_array_push(&argv, &arg);
    }
    int exit_code = EDG_MAIN(argc, (char **)argv.data);
    write(ticket->communication_fd, end_of_message_bytes, strlen(end_of_message_bytes));
    edg_string_append_i64(&exit_message, exit_code);
    edg_string_append_cstr(&exit_message, end_of_message_bytes);
    write(ticket->communication_fd, exit_message.data, exit_message.len);
  }
  goto done;
client_read_failure:
  fputs("Received incomplete message from client.\n", stderr);
done:
  edg_array_destroy(&argv);
  edg_string_destroy(&exit_message);
  edg_string_destroy(&arguments);
  edg_string_destroy(&request);
}


static void *run_front_end_thread(void *unused)
/*
Run a front end thread that waits for work from the any_work pthread condition.
*/
{
  while (TRUE) {
    /* Acquire the any work lock and wait to be signaled that there's new
       work. */
    int acquired_lock = pthread_mutex_lock(&next_ticket_lock);

    if (acquired_lock != 0) {
      goto terminate_thread;
    }  /* if */
    /* Wait on the ticket becoming available. */
    while (next_ticket == NULL) {
      int wait_error = pthread_cond_wait(&any_work, &next_ticket_lock);
      if (wait_error != 0) {
        goto terminate_thread;
      }  /* if */
    }  /* while */

    /* Take the ticket. */
    an_invocation_ticket *current_ticket = next_ticket;
    next_ticket = NULL;

    /* Release the lock so more work can be assigned to other threads. */
    int released_lock = pthread_mutex_unlock(&next_ticket_lock);
    if (released_lock != 0) {
      destroy_ticket(current_ticket);
      goto terminate_thread;
    }  /* if */
    process_ticket(current_ticket);
    destroy_ticket(current_ticket);
  }  /* while */
terminate_thread:
#if DEBUG
  edg_string_destroy(&trans_unit_tag);
#endif
  fputs("Thread shutting down.\n", stderr);
  return NULL;
}  /* run_front_end_thread */


static int determine_parallelism()
/*
Return the maximum number of threads to run in the daemon.

When MULTIPLE_THREAD_COMPILATION is TRUE: (for ease of implementation) this is
always the number of processors (i.e., threads) the host system has.

When MULTIPLE_THREAD_COMPILATION is FALSE: this is always one (to ensure only
one front end invocation occurs at a time within this process).
*/
{
#if MULTIPLE_THREAD_COMPILATION
  long num_processors = sysconf(_SC_NPROCESSORS_ONLN);

  /* Include an extra thread so that when clients assign one front end
     execution per system thread there is a thread available to answer
     pings. */
  return ((int)num_processors) + 1;
#else /* !MULTIPLE_THREAD_COMPILATION */
  return 1;
#endif /* MULTIPLE_THREAD_COMPILATION */
}  /* determine_parallelism */

/* Conditionally close the "edg" namespace. */
END_EDG_NAMESPACE

/*
Note that the EDG namespace is not opened here.  Instead we do a
using-directive if the EDG code is in the edg namespace.
*/
USING_NAMESPACE_EDG

int main(int argc, char *argv[])
/*
The main routine for the front end Unix socket daemon.
*/
{
  int          return_value = 0;
  int          max_threads = determine_parallelism();
  pthread_t    *threads = (pthread_t *)malloc(sizeof(*threads) * max_threads);
  if (threads == NULL) abort();
  a_const_char *socket_address = allocating_socket_file_name();
  int          init_progress = 0;

  next_ticket = NULL;
  { /* Create the threads that wait for work on a shared condition (any_work);
       this is done ahead of time to allow for threads to be reused rather than
       creating a new thread for each invocation and/or "searching" for the
       next available thread. */
    int next_ticket_lock_set_up = pthread_mutex_init(&next_ticket_lock,
                                                     /*attr=*/NULL);

    if (next_ticket_lock_set_up != 0) {
      goto threading_set_up_error;
    }  /* if */
    ++init_progress;

    int any_work_set_up = pthread_cond_init(&any_work, /*attr=*/NULL);
    if (any_work_set_up != 0) {
      goto threading_set_up_error;
    }  /* if */
    ++init_progress;

    /* Set up the threading attributes. */
    pthread_attr_t thread_attr;
    if (pthread_attr_init(&thread_attr) != 0) {
      goto threading_set_up_error;
    }  /* if */

    /* Set the thread's stack to approximately 10MiB (the normal stack size on
       most Linux main threads).  Some operating systems (e.g., MacOS) mandate
       that the thread stack size is a multiple of the page size; thus, the
       10MiB starting point is adjusted so as to be a multiple of the page
       size. */
    size_t stack_size = 10 * 1024 * 1024;
    size_t page_size = (size_t)getpagesize();
    size_t page_remainder = stack_size % page_size;
    if (page_remainder != 0) {
      stack_size -= page_remainder;
      stack_size += page_size;
    }  /* if */
    if (pthread_attr_setstacksize(&thread_attr, stack_size) != 0) {
      goto threading_set_up_error;
    }  /* if */
    for (int i = 0; i < max_threads; i++) {
      int thread_launch_result = pthread_create(&threads[i], &thread_attr,
                                                run_front_end_thread,
                                                /*arg=*/NULL);
      if (thread_launch_result != 0) {
        goto threading_set_up_error;
      }  /* if */
    }  /* for */
    /* Destroy the threading attributes. */
    if (pthread_attr_destroy(&thread_attr) != 0) {
      goto threading_set_up_error;
    }  /* if */

    /* Create a new Unix streaming (bi-directional) socket. */
    int server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
      goto socket_set_up_error;
    }  /* if */

    /* Attempt to remove any remaining named socket file from a previous daemon
       invocation. */
    (void)unlink(socket_address);

    /* Bind the socket (so that the client can find it) and begin listening for
       connections. */
    size_t      socket_path_bytes = strlen(socket_address) + 1;
    struct sockaddr_un server_socket_addr = {0};
    if (socket_path_bytes > sizeof(server_socket_addr.sun_path)) {
      goto socket_path_too_long_error;
    }  /* if */
    server_socket_addr.sun_family = AF_UNIX;
    memcpy(server_socket_addr.sun_path, socket_address, socket_path_bytes);

    int bind_result = bind(server_socket_fd,
                           (const struct sockaddr*)&server_socket_addr,
                           sizeof(server_socket_addr));
    if (bind_result == -1) {
      goto socket_set_up_error;
    }  /* if */

    /* Determine how large of a backlog to offer dynamically based on the
       number of processors (i.e., threads) the host system supports.  This
       allows a reasonably sane number of requests to quickly hit the daemon
       before it applies back-pressure to the client (via the kernel denying
       the connection requests). */
    int backlog_size = max_threads * 2;
    int listen_result = listen(server_socket_fd, backlog_size);
    if (listen_result == -1) {
      goto socket_set_up_error;
    }  /* if */
    /* Begin accepting connections. */
    while (TRUE) {
      /* Listen for new work. */
      int       accept_socket_fd = accept(server_socket_fd, /*address=*/NULL,
                                          /*address_len=*/NULL);
      a_boolean try_again = TRUE;

      /* Attempt to assign the work. */
      do {
        int acquired_lock = pthread_mutex_lock(&next_ticket_lock);

        if (acquired_lock != 0) {
          goto coordination_lock_error;
        }  /* if */
        if (next_ticket == NULL) {
          /* Allocate a new ticket; it will be freed by the thread that accepts
             the work. */
          next_ticket = (an_invocation_ticket *)malloc(sizeof(*next_ticket));
          if (next_ticket == NULL) abort();
          next_ticket->communication_fd = accept_socket_fd;
          try_again = FALSE;
        }  /* if */
        /* If there's a sleeping worker thread available, let it know there's
           work to be done. */
        pthread_cond_signal(&any_work);

        int released_lock = pthread_mutex_unlock(&next_ticket_lock);
        if (released_lock != 0) {
          goto coordination_lock_error;
        }  /* if */
      } while (try_again);
    }  /* while */
  }
  goto done;
threading_set_up_error:
  fputs("Thread set up failed.\n", stderr);
  return_value = 1;
  goto done;
socket_set_up_error:
  fputs("Unix socket set up failed.\n", stderr);
  fprintf(stderr, "Attempting to use socket: %s\n", socket_address);
  return_value = 1;
  goto done;
socket_path_too_long_error:
  fprintf(stderr, "Unix socket path (\"%s\") is too long.\n", socket_address);
  return_value = 1;
  goto done;
coordination_lock_error:
  fputs("Coordination lock failure.\n", stderr);
  return_value = 1;
  goto done;
done:
  if (init_progress > 1) {
    pthread_cond_destroy(&any_work);
  }  /* if */
  if (init_progress > 0) {
    pthread_mutex_destroy(&next_ticket_lock);
  }  /* if */
  destroy_ticket(next_ticket);
  free(threads);
  free((void *)socket_address);
  return return_value;
}  /* main */

#endif /* EDG_FRONT_END_DAEMON */


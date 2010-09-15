#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/select.h>

#include <libfixposix.h>

static inline void error_abort (const char* msg, int perrorp) {
        if (perrorp) {
                perror(msg);
        } else {
                fprintf(stderr,"%s\n",msg);
        }
        abort();
}

extern int main (int argc, char *argv[]) {
        // Not implemented yet
        pid_t pid;
        char *path = "/bin/sh";
        char *myargv[] = {"/bin/sh","-c","echo foo ; echo bar >&2 ; cat",NULL};
	lfp_spawn_file_actions_t file_actions;
        lfp_spawnattr_t attr;
        int err, status;
        char tempnam1[] = {"/tmp/lfp_spawn_test_1_XXXXXX\0"};
        char tempnam2[] = {"/tmp/lfp_spawn_test_2_XXXXXX\0"};
        int tempfd1, tempfd2;
	char buf[256];
	char expected[] = {"foo\nbar\nquux\n"};

        if ( (tempfd1 = mkstemp(tempnam1)) == -1 ) {
                error_abort("failed to create temp file 1",1);
        }
        if ( (tempfd2 = mkstemp(tempnam2)) == -1 ) {
                error_abort("failed to create temp file 2",1);
        }
        if (write(tempfd1,"quux\n",5)!=5) {
                error_abort("failed to initialize temp file 1", 1);
        }
        if (close(tempfd1) == -1) {
                error_abort("failed to close temp file 1", 1);
        }
        if (close(tempfd2) == -1) {
                error_abort("failed to close temp file 2", 1);
        }
        err = lfp_spawn_file_actions_init(&file_actions);
        if (err) { error_abort("failed to init file actions", 1); }
        err = lfp_spawnattr_init(&attr);
        if (err) { error_abort("failed to init attr", 1); }
        err = lfp_spawn_file_actions_addopen(&file_actions,10,tempnam1,O_EXCL|O_RDONLY,0);
        err = lfp_spawn_file_actions_adddup2(&file_actions,10,0);
        err = lfp_spawn_file_actions_addclose(&file_actions,10);
        err = lfp_spawn_file_actions_addopen(&file_actions,1,tempnam2,O_WRONLY|O_APPEND,0);
        err = lfp_spawn_file_actions_adddup2(&file_actions,1,2);
        err = lfp_spawn(&pid,path,myargv,environ,&file_actions,&attr,false);
        if (err) { error_abort("failed to spawn", 1); }
        fprintf(stderr,"spawned child %d\n",pid);
        err = lfp_spawn_file_actions_destroy(&file_actions);
        if (err) { error_abort("failed to init file actions", 1); }
        err = lfp_spawnattr_destroy(&attr);
        if (err) { error_abort("failed to init attr", 1); }
        err = waitpid(pid,&status,0);
        if (err == -1) { error_abort("failed to wait for child", 1); }
        if (!WIFEXITED(status)) { error_abort("child didn't exit properly", 0); }
        if (WEXITSTATUS(status)) { error_abort("child didn't exited with status 0", 0); }
        tempfd2 = open(tempnam2, O_RDONLY);
        if (tempfd2==-1) { error_abort("failed to open the output file", 1); }
	err = read(tempfd2,buf,sizeof(buf));
	if (err == -1) { error_abort("failed to read the output file",1); }
	if (err != (sizeof(expected)-1)) {
		fprintf(stderr,"expected %u bytes, got %d\n",sizeof(expected)-1,err);
		error_abort("found the wrong number of bytes", 0);
	}
        err = strncmp(expected,buf,err);
        if (err) {
		error_abort("output doesn't match expectation",0);
	}
	err = unlink(tempnam1);
	if (err == -1) { error_abort("failed to remove the input file",1); }
	unlink(tempnam2);
	if (err == -1) { error_abort("failed to remove the output file",1); }
	fprintf(stderr,"spawn test successful\n");
        exit(0);
}

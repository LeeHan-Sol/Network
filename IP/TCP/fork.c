#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 0; /* 전역 변수 선언 */
int main(void) {
	pid_t pid;
	int local_var = 0; /* 지역 변수 선언 */

	if((pid = fork()) < 0) 
	{
		printf("fork error\n");
		return -1;
		/* 자식 프로세스 */
	} 
	else if (pid == 0) 
	{
		global_var++;
		local_var++;
		printf("CHILD - my pid : %d, parent's pid : %d\n", getpid(), getppid());
	} 
	else 
	{
		/* 부모 프로세스 */
		sleep(2); /* 2초 쉰다 */
		global_var += 5;
		local_var += 5;
		printf("PARENT - my pid is %d, child's pid is %d\n", getpid(), pid);
	}
	printf("\t global var : %d\n", global_var);
	printf("\t local var : %d\n", local_var);

}

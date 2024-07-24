#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
int err(char *s){
	while(*s)
		write(2,s++,1);
	return 1;
}
int cd(char **av,int i){
	if (i!=2)
		return err("error: cd: bad arguments\n");
	if (chdir(av[1])==-1)
		return err("error: cd: cannot change directory to "),err(av[1]),err("\n");
	return 0;
}
int ex(char **av,int i,char **envp){
	int fd[2];
	int status;
	int haspipe=av[i]&&!strcmp(av[i],"|");
	if (!haspipe && !strcmp(*av,"cd"))
		return cd(av,i);
	if (haspipe&&pipe(fd)==-1)
		return err("error: fatal\n");
	int pid=fork();
	if (pid==0){
		av[i]=0;
		if (haspipe && (dup2(fd[1],1)==-1 || close(fd[0])==-1 || close(fd[1])== -1))
			return err("error: fatal\n");
		if (!strcmp(*av,"cd"))
			return (cd(av,i));
		execve(*av,av,envp);
		return err("error: cannot execute "),err(*av),err("\n"); 
	}
	waitpid(pid,&status,0);
	if (haspipe && (dup2(fd[0],0)==-1 || close(fd[0])== -1 || close(fd[1])== -1))
		return err("error: fatal\n");
	return (WIFEXITED(status) && WEXITSTATUS(status));
}
int main(int ac,char **av,char** envp){
	int status=0;
	int i=0;
	if (ac>1){
		while(av[i]&&av[++i]){
			av+=i;
			i=0;
			while(av[i]&&strcmp(av[i],"|")&&strcmp(av[i],";"))
				i++;
			if (i)
				status = ex(av,i,envp);
		}
	}
	return status;
}
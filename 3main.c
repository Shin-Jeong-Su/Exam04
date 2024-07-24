#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
int err(char *s){
	while(*s){
		write(2,s++,1);
	}
	return 1;
}
int cd(char **av,int i){
	if (i>2)
		return err("hh");
	if (chdir(av[i])==-1)
		return (err("hh"));
	return 0;
}
int ex(char **av,int i,char **envp){
	int fd[2];
	int status;
	int haspipe=av[i]&&!strcmp(av[i],"|");
	if (haspipe&&pipe(fd)==-1)
		return err("hh");
	if (!haspipe && !strcmp(*av,"cd"))
		return (cd(av,i));
	int pid=fork();
	if (pid==0){
		av[i]=0;
		if (haspipe && (dup2(fd[1],1) || close(fd[1]) || close(fd[0])))
			return err("hh");
		if (!strcmp(*av,"cd"))
			return cd(av,i);
		execve(*av,av,envp);
		return err("hh");
	}
	waitpid(pid,&status,0);
	if (haspipe&&(dup2(fd[0],0)==-1 || close(fd[0] == -1 || close(fd[1] == -1))))
		return (err("hh"));
	return WIFEXITED(status)&&WEXITSTATUS(status);
}
int main(int ac,char **av,char **envp){
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
	return (status);
}
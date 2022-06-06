#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;
  addr = p->sz;
  p->sz += n;
  if(n < 0) {
    p->sz = uvmdealloc(p->pagetable, addr, p->sz);
  }
  
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace();
  
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sysinfo(void)
{
  uint64 infoaddr;
  struct sysinfo info;
  struct proc *p = myproc();

  if(argaddr(0, &infoaddr) < 0)
    return -1;

  info.freemem = kavail() * PGSIZE;
  info.nproc = proccount();
  
  if(copyout(p->pagetable, infoaddr, (char*)&info, sizeof(struct sysinfo)) < 0)
    return -1;

  return 0;
}

uint64 sys_trace(void)
{
  int tracemask;
  struct proc *p = myproc();
  
  if(argint(0, &tracemask) < 0)
    return -1;

  p->tracemask = tracemask;
  
  return 0;
}


uint64
sys_getuid(void)
{
  return myproc()->uid;
}

uint64
sys_setuid(void)
{
  int uid;

  if (argint(0, &uid) < 0)
    return -1;
  
  myproc()->uid = uid;
  return 0;
}

uint64
sys_getgid(void)
{
  return myproc()->gid;
}

uint64
sys_setgid(void)
{
  int gid;

  if (argint(0, &gid) < 0)
    return -1;
  
  myproc()->gid = gid;
  return 0;
}

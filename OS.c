#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"list.h"
int  number_of_pid=0;



typedef struct message
{
  int pid_of_sender;
  int status;
  char mess[40];
}message;

typedef struct PCB
{
    int pid;
    int priority;
    int sender_pid;
    int status;
    char content[40];
}PCB;

typedef struct Semaphore
{
  int ID;
  int value;
  LIST *waiting;
}Sem;

LIST* PROcesses;
LIST* Ready_0;
LIST* Ready_1;
LIST* Ready_2;
LIST* waiting_for_reply;
LIST* waiting_for_receive;
PCB *Run,*init, *p,*q,*r;
message *be_sent;

Sem *Sem0,*Sem1,*Sem2,*Sem3,*Sem4;

int comparator_pid(const PCB *a, int b)
{
    if(a==NULL)
      return 0;
    if (a->pid==b)
      return 1;
    else
     return 0;
}

int comparator(const PCB *a, const PCB *b)
{
    if(a==NULL)
    return 0;
    if (a==b)
      return 1;
    else
     return 0;
}

int enqueue_Ready(PCB* a)
{
  int i;
  if (a->priority==0)
    i=ListAppend(Ready_0, a);
  if (a->priority==1)
    i=ListAppend(Ready_1, a);
  if (a->priority==2)
    i=ListAppend(Ready_2, a);;
  return i;
}


void dequeue_Ready(PCB* a)
{
  if (a->priority==0)
  {
    ListSearch(Ready_0, (void*)comparator , (void*)a);
    ListRemove(Ready_0);
  }
  if (a->priority==1)
  {
    ListSearch(Ready_1, (void*)comparator , (void*)a);
    ListRemove(Ready_1);
  }
  if (a->priority==2)
  {
    ListSearch(Ready_2, (void*)comparator , (void*)a);
    ListRemove(Ready_2);
  }
}

int compare()
{
    int prun,Prio;
    Prio =100;
    if (ListCount(Ready_0) != 0)
    {
      p= (PCB*)ListLast(Ready_0);
      Prio=0;
    }
    else if (ListCount(Ready_1) != 0)
    {
      p=(PCB*)ListLast(Ready_1);
      Prio=1;
    }
    else if (ListCount(Ready_2) != 0)
    {
      p=(PCB*)ListLast(Ready_2);
      Prio=2;
    }
    else if (Run == NULL)
      return 0;
    if (Run == NULL)
    {
      Run = p;
      Run->status=1;
      return 1;
    }
    prun=Run->priority;
    if(Prio<prun)
      {
      Run->status=2;
      enqueue_Ready(Run);
      dequeue_Ready(p);
      Run=p;
      Run->status=1;
      return 1;
      }
    else
      return 0;
}

int execute()
{
  int i = 1;
  if(Run == NULL && ListCount(Ready_0) == 0 && ListCount(Ready_1) == 0 && ListCount(Ready_2) == 0)
  {
       Run=init;
       Run->status=1;
  }
  else if (Run == NULL && ListCount(Ready_0) != 0)
  {
    Run= (PCB*)ListLast(Ready_0);
  }
  else if (Run == NULL && ListCount(Ready_1) != 0)
  {
    Run= (PCB*)ListLast(Ready_1);
  }
  else if (Run == NULL && ListCount(Ready_2) != 0)
  {
    Run= (PCB*)ListLast(Ready_2);
  }
  else if(compare()==0)
    i =0;
  return i;
}

void go_to_first()
{
  ListFirst(PROcesses);
  ListFirst(Ready_0);
  ListFirst(Ready_1);
  ListFirst(Ready_2);
  ListFirst(waiting_for_reply);
  ListFirst(waiting_for_receive);
}

int create(int Priority)
{
    int i= 0;
            p=NULL;
            number_of_pid++;
            p=(PCB*)malloc(sizeof(PCB));
            p->sender_pid=-1;
            p->pid=number_of_pid;
            p->priority=Priority;
            strcpy(p->content, "\0");
            p->status=2;
            ListAppend(PROcesses,p);
            enqueue_Ready(p);
            i= (ListCurr(PROcesses)==p);
            if(i)
              printf("PID=%d has been created!\n", p->pid);
    return i;
}

void Fork()
{
  number_of_pid++;
  p=Run;
  q=(PCB*)malloc(sizeof(PCB));
  strcpy(q->content, p->content);
  q->pid=number_of_pid;
  q->priority=p->priority;
  q->sender_pid=-1;
  q->status=2;
  ListAppend(PROcesses,q);
  enqueue_Ready(q);
}



int Exit()
{
     go_to_first();
     struct PCB *Delete_one=Run;
     Run=NULL;
     int i = Delete_one->pid;
     ListSearch(PROcesses, (void*)comparator , (void*)Delete_one);
     ListRemove(PROcesses);
     free(Delete_one);
     Delete_one=NULL;
     execute();
     printf("The running process of PID %d has been killed!\n", i);
     return i;
}

int kill(int PID)
{
    go_to_first();
    int i=0;
    struct PCB *Delete_one;
    Delete_one = ListSearch(PROcesses, (void*)comparator_pid , (void*)PID);
    if (Delete_one==NULL)
    {
      printf("The process is not exist!\n");
      return i;
    }
    ListRemove(PROcesses);
    if (Delete_one->status==1)
      Run=NULL;
    if (Delete_one->status==2)
      dequeue_Ready(p);
    if (Delete_one->status==3)
    {
      ListSearch(waiting_for_receive, (void*)comparator , (void*)Delete_one);
      ListRemove(waiting_for_receive);
      ListSearch(waiting_for_reply, (void*)comparator , (void*)Delete_one);
      ListRemove(waiting_for_reply);
    }
    free(Delete_one);
    Delete_one=NULL;
    execute();
    go_to_first();
    if (Delete_one == NULL)
    {
      i = 1;
      printf("The process has been killd!\n");
    }
    else
      printf("Failed to kill!\n");
    return i;
}


void send(int PID,  char *msg)
{
    go_to_first;
    if (PID ==0)
      p = init;
    else{
      p = ListSearch(PROcesses, (void*)comparator_pid , (void*)PID);
    if (p==NULL)
    {
      printf("The process is not exist!\n");
      return ;
    }
    if(p->status==2)
      dequeue_Ready(p);
    else if(p->status==1)
    {
      Run=NULL;
      execute();
    }
    p->status=3;}
    ListAppend(waiting_for_reply,p);
    if (ListCount (waiting_for_receive)==0)
    {
      if(be_sent==NULL)
      {
        message *new_content=(message*)malloc(sizeof(message));
        be_sent=new_content;
      }
      strcpy(be_sent->mess, msg);
      be_sent->status=0;
      be_sent->pid_of_sender=PID;
      printf("Sent!\n");
    }
    else
    {
      q=ListLast(waiting_for_receive);
      ListSearch(waiting_for_receive, (void*)comparator , (void*)q);
      ListRemove(waiting_for_receive);
      strcpy(q->content, msg);
      q->status=2;
      q->sender_pid=PID;
      enqueue_Ready(q);
      printf("Sent!\n");
      execute();
    }
}


int Quantum(){
  int i;
	if(Run->pid == init->pid){
		printf("Init Process is running now\n");
    return 0;
	}
	else{
		if(ListCount(Ready_0) == 0 && ListCount(Ready_1) == 0 && ListCount(Ready_2) == 0){
			printf("Only one process in the OS now.\n");
      return 0;
		}
	else if(ListCount(Ready_0) >0)
  {
			Run->status = 2;
			enqueue_Ready(Run);
			ListFirst(Ready_0);
			Run = (PCB*) ListRemove(Ready_0);
			Run->status = 1;
      i=Run->pid;
		}
		else if(ListCount(Ready_1) >0){
			Run->status = 2;
			enqueue_Ready(Run);
			ListFirst(Ready_1);
			Run = (PCB*) ListRemove(Ready_1);
			Run->status = 1;
      i=Run->pid;
		}
		else if(ListCount(Ready_2) >1){
			Run->status = 2;
			enqueue_Ready(Run);
			ListFirst(Ready_2);
			Run = (PCB*) ListRemove(Ready_2);
			Run->status = 1;
      i=Run->pid;
		}
		else{
			printf("Failed to kill the process.\n");
      return 0;
		}
    return i;
	}
}

int receive()
{
  if(Run==NULL)
  {
    printf("No processes is running!");
    return 0;
  }
  if (be_sent->status == 0)
  {
    p=Run;
    strcpy(p->content, be_sent->mess);
    be_sent->status = 1;
    p->sender_pid=be_sent->pid_of_sender;
    return p->pid;
  }
  else
  {
    if (Run->pid == 0)
      return -2;
    Run->status=3;
    p= Run;
    ListAppend(waiting_for_receive,p);
    Run=NULL;
    execute();
    return -1;
  }
}

int reply(int PID,  char *msg)
{
  go_to_first();
  p = ListSearch(PROcesses, (void*)comparator_pid , (void*)PID);
  if (p==NULL)
  {
    printf("The process is not exist!\n");
    return 0;
  }
  if (p->sender_pid==0)
  {
    printf("The process has not received any message!\n");
    return 0;
  }
  q = ListSearch(waiting_for_reply, (void*)comparator_pid , (void*)p->sender_pid);
  if (q==NULL)
  {
    printf("The sender has been killed!\n");
    return 0;
  }
  ListRemove(waiting_for_reply);
  strcpy(q->content, msg);
  q->status=2;
  enqueue_Ready(q);
  printf("Replied!\n");
  execute();
  return 1;
}

int New_Semaphore(int semaphore, int value)
{
    if (semaphore==0)
    {
      Sem0=(Sem*)malloc(sizeof(Sem));
      Sem0->ID=semaphore;
      Sem0->value=value;
      Sem0->waiting=ListCreate();;
    }
    else if (semaphore==1)
    {
      Sem1=(Sem*)malloc(sizeof(Sem));
      Sem1->ID=semaphore;
      Sem1->value=value;
      Sem1->waiting=ListCreate();;
    }
    else if (semaphore==2)
    {
      Sem2=(Sem*)malloc(sizeof(Sem));
      Sem2->ID=semaphore;
      Sem2->value=value;
      Sem2->waiting=ListCreate();;
    }
    else if (semaphore==3)
    {
      Sem3=(Sem*)malloc(sizeof(Sem));
      Sem3->ID=semaphore;
      Sem3->value=value;
      Sem3->waiting=ListCreate();;
    }
    else if (semaphore==4)
    {
      Sem4=(Sem*)malloc(sizeof(Sem));
      Sem4->ID=semaphore;
      Sem4->value=value;
      Sem4->waiting=ListCreate();;
    }
    else
    {
      printf("illegal semaphore ID! \n");
      return 0;
    }
    return 1;
}

int Sempahore_P(int semaphore)
{
  PCB* sp = Run;
  if (semaphore==0)
  {
    if (Sem0->value<=0)
    {
      sp->status=3;
      ListAppend(Sem0->waiting, sp);
      Run=NULL;
    }
    else
      Sem0->value--;
  }
  else if (semaphore==1)
  {
    if (Sem1->value<=0)
    {
      sp->status=3;
      ListAppend(Sem1->waiting, sp);
      Run=NULL;
    }
    else
      Sem1->value--;
  }
  else if (semaphore==2)
  {
    if (Sem2->value<=0)
    {
      sp->status=3;
      ListAppend(Sem2->waiting, sp);
      Run=NULL;
    }
    else
      Sem2->value--;
  }
  else if (semaphore==3)
  {
    if (Sem3->value<=0)
    {
      sp=Run;
      sp->status=3;
      ListAppend(Sem3->waiting, sp);
      Run=NULL;
    }
    else
      Sem3->value--;
  }
  else if (semaphore==4)
  {
    if (Sem4->value<=0)
    {
      sp->status=3;
      ListAppend(Sem4->waiting, sp);
      Run=NULL;
    }
    else
      Sem4->value--;
  }
  else
  {
    printf("illegal semaphore ID! \n");
    return 0;
  }
  if(sp->status==3)
    return 1;
  else return 0;
  return 0;
}

int Sempahore_V(int semaphore)
{
  PCB* sp = NULL;
  if (semaphore==0)
  {
    Sem0->value++;
    if (Sem0->value>=1)
    {
      if (ListCount(Sem0->waiting)>0)
      {
        sp=ListLast(Sem0->waiting);
        ListRemove(Sem0->waiting);
        sp->status=2;
        enqueue_Ready(sp);
      }
      printf("%d\n",ListCount(Sem0->waiting));
    }
  }
  else if (semaphore==1)
  {
    Sem1->value++;
    if (Sem1->value>=1)
    {
      if (ListCount(Sem1->waiting)>0)
      {
      sp=ListLast(Sem1->waiting);
      ListRemove(Sem1->waiting);
      sp->status=2;
      enqueue_Ready(sp);
      }
    }
  }
  else if (semaphore==2)
  {
    Sem2->value--;
    if (Sem2->value>=1)
    {
      if (ListCount(Sem2->waiting)>0)
      {
      sp=ListLast(Sem2->waiting);
      ListRemove(Sem2->waiting);
      sp->status=2;
      enqueue_Ready(sp);
      }
    }
  }
  else if (semaphore==3)
  {
    Sem3->value++;
    if (Sem3->value>=1)
    {
      if (ListCount(Sem3->waiting)>0)
      {
      sp=ListLast(Sem3->waiting);
      ListRemove(Sem3->waiting);
      sp->status=2;
      enqueue_Ready(sp);
      }
    }
  }
  else if (semaphore==4)
  {
    Sem4->value++;
    if (Sem4->value>=1)
    {
      if (ListCount(Sem4->waiting)>0)
      {
      sp=ListLast(Sem4->waiting);
      ListRemove(Sem4->waiting);
      sp->status=2;
      enqueue_Ready(sp);
      }
    }
  }
  else
  {
    printf("illegal semaphore ID! \n");
    return 0;
  }
  if (sp==NULL)
    return 0;
  if (sp->status==2)
    return 1;
  else return 0;
}






void Procinfo(int PID)
{
    go_to_first();
    p = ListSearch(PROcesses, (void*)comparator_pid , (void*)PID);
    if (p==NULL)
    {
      printf("The process is not exist!\n");
      return;
    }
    printf("The PID of this process is :%d\n",p->pid);
    printf("The priority of this process is:%d\n",p->priority);
    char s[20];
    if(p->status==1)
      strcpy(s, "Running");
    else if(p->status==2)
      strcpy(s, "Ready");
    else if(p->status==3)
      strcpy(s, "Blocked");
    printf("The status of this process is:%s\n",s);
    printf("The content of this process is :\n");
    printf("%s",p->content);
    printf("\n");
    go_to_first();
    return;
}

void queue_output(LIST* liST)
{
  go_to_first();
  int st=ListCount(liST);
  while(st !=0)
  {
    p=ListCurr(liST);
    printf("%dth of this queue:",st);
    printf("The PID of this process is :%d\n",p->pid);
    printf("The priority of this process is:%d\n",p->priority);
    char s[20];
    if(p->status==1)
      strcpy(s, "Running");
    else if(p->status==2)
      strcpy(s, "Ready");
    else if(p->status==1)
      strcpy(s, "Blocked");
    printf("The status of this process is:%s\n",s);
    printf("The content of this process is :\n");
    printf("%s",p->content);
    printf("\n");
    st--;
    ListNext(liST);
  }
}

void Totalinfo()
{
  printf("Totalinfo! \n");
  printf("***************\n");
  printf("\n");
  printf("The Ready queue of priority 0: \n");
  queue_output(Ready_0);
  printf("The Ready queue of priority 1: \n");
  queue_output(Ready_1);
  printf("The Ready queue of priority 2: \n");
  queue_output(Ready_2);
  printf("The queue of waiting_for_reply: \n");
  queue_output(waiting_for_reply);
  printf("The queue of waiting_for_receive: \n");
  queue_output(waiting_for_receive);
  printf("**********************\n");
}

int main()
{
    int k,j,s;
    char ch;
    char mes[40];
    int DO=1;

    Run=NULL;
    be_sent=NULL;
    PROcesses=ListCreate();
    Ready_0=ListCreate();
    Ready_1=ListCreate();
    Ready_2=ListCreate();
    waiting_for_reply=ListCreate();
    waiting_for_receive=ListCreate();


    init=(PCB*)malloc(sizeof(PCB));
    strcpy(init->content, "\0");
    init->pid=0;
    init->priority=3;
    init->sender_pid=-1;
    init->status=1;
    Run=init;
    ListAppend(PROcesses,init);

    while(DO==1)
    {

        if (init->status==3)
        {
          ListSearch(waiting_for_receive, (void*)comparator , (void*)init);
          ListRemove(waiting_for_receive);
          ListSearch(waiting_for_reply, (void*)comparator , (void*)init);
          ListRemove(waiting_for_reply);
          enqueue_Ready(init);
        }
        execute();

        printf("********************************************\n");
        printf("* PCB’s and Process Scheduling Simulation  *\n");
        printf("********************************************\n");
        printf("     C.Create          I.Procinfo \n");
        printf("     Q.Quantum         E.Exit \n");
        printf("     S.send            R.receive \n");
        printf("     F.Fork            K.kill \n");
        printf("     Y.Reply           T.Totalinfo \n");
        printf("     N.NewSemaphore    P.SempahoreP \n");
        printf("     V.SempahoreV      0.End \n");
        printf("********************************************\n");
        printf("Please choose the function you want (Capital letter): \n");
        scanf("%s",&ch);
        if (ch == 'C'){
              int check = 1;
              int pri;
              while (check)
              {
                printf("Please type the priority:\n");
                scanf("%d",&pri);
                if (pri != 0 && pri != 1 && pri != 2)
                  printf("Illegal input! Please try again!\n");
                else
                  check =0;
              }
              k=create(pri);
              if(k==1)
              {
                  printf("created successfully!\n");
              }
              k=compare();
              if(k==1)
              {
                  printf("Running the new process because it has higher priority\n");
              }
              if(k!=1)
                  printf("The new process has lower priority so the current running process is still running\n");
                }
        else if (ch == 'I'){
              printf("Please type the PID you want to show:\n");
              int pri;
              scanf("%d",&pri);
              Procinfo(pri);
            }
    else if(ch =='Q'){
      j = Quantum();
      if (j!=0)
        printf("The pid of %d is running now!\n", j);
    }

    else if (ch == 'K'){
        int check =1;
        while(check)
        {
          printf("Please type the PID you want to kill:\n");
          scanf("%d",&k);
          if (k>0)
            check = 0;
          else if (k==0)
            printf("You cannot kill the init process!" );
          else
            printf("Illegal input!" );
          }
        kill(k);
      }
    else if (ch == 'E'){
         if(Run==NULL)
         {
             printf("No process is running!\n");
             continue;
         }
         if(PROcesses==NULL)
         {
             printf("No process now!Please create new one!\n");
             continue;
         }
         k=Exit();
         if (k==0)
         {
          printf("init process has been killed.\n");
          printf("Exit to this OS\n");
          DO=0;
          }
         else
         {
           j=execute();
           if(j==1)
           {
              printf("Running PID of %d now!\n", Run->pid);
           }
         }
       }
     else if (ch == 'S'){
       if(Run==NULL)
       {
           printf("No process is running!\n");
           continue;
       }
       if(PROcesses==NULL)
       {
           printf("No process now!Please create new one!\n");
           continue;
       }
        printf("Please type the PID of sender in: \n");
        scanf("%d",&s);
        printf("Please type the Message in: \n");
        scanf("%s",mes);
        send(s, mes);
        j=execute();
        if(j==1)
        {
             printf("Running another process now!\n");
        }
      }
      else if (ch == 'R'){
        k=receive();
        if(k==-1)
          printf("No message! The running one has been blocked! \n");
        else if(k>0)
        {
          printf("receive successfully!  \n");
          printf("The PID of the Receiver is %d!  \n", k);
          printf("Receive from %d!  \n", Run->sender_pid);
          printf("Content is:  \n");
          printf("\"%s\"  \n", Run->content);
        }
      }
      else if (ch == 'Y'){
        printf("Please type the PID of sender in: \n");
        scanf("%d",&s);
        printf("Please type the Message in: \n");
        scanf("%s",mes);
        k=reply(s, mes);
    }
    else if (ch == 'T'){
        Totalinfo();
    }
    else if (ch == 'N'){
      printf("Please type in the ID of Semaphore(1~5): \n");
      scanf("%d",&s);
      printf("Please type in the value of Semaphore: \n");
      scanf("%d",&j);
      New_Semaphore(s, j);
    }
    else if (ch == 'P'){
      printf("Please type in the ID of Semaphore(1~5): \n");
      scanf("%d",&s);
      j = Sempahore_P(s);
      if (j)
        printf("the running process has been blocked!\n");
    }
    else if (ch == 'V'){
      printf("Please type in the ID of Semaphore(1~5): \n");
      scanf("%d",&s);
      j=Sempahore_V(s);
      if (j)
        printf("the process has been unblocked!\n");
    }

    else if(ch =='0')
      DO=0;
    else
      printf("illegal input!\n");}
    return 0;
}

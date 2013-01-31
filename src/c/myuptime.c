/*

myuptime.c - linux kernel module - 1.0
changes the current system uptime in linux 2.4.x
by Tiago Sousa <mirage@kaotik.org> May 2003

>> compile

gcc -O2 -Wall -isystem /lib/modules/`uname -r`/build/include -c myuptime.c

>> usage

insmod myuptime.o <command> [options] && rmmod myuptime

commands:
newjiffies=<jiffies>		change the uptime to <jiffies> directly
or
newseconds=<seconds>		change the uptime to <seconds>
or
newdays=<days>			change the uptime to <days>

options:
log=0				it won't printk() any info
interruptnum=<num>		change jiffies on each <num> timer interrupts
				(default 50, don't change unless you know what
				you're doing)
jiffiesinc=<num>		number of jiffies to add on each timer
				(default 10000000, again you shouldn't touch
				it unless..)

>> example

insmod myuptime.o newdays=69 log=0 && rmmod myuptime

the rmmod will be suspended while the uptime increases until it gets to the
chosen value, which may take from a few seconds to a couple of minutes. note
that if you go "back" in time, it will first increase until the jiffies counter
overflows (returning to 0 uptime) and then continue up to the final chosen
uptime. it can't be instantly updated because kernel timers go crazy when the
time warp is too large (tcp connections die, for example). even with this
method, i've found that problems may still arise (decreasing interruptnum and
jiffiesinc can help if you want to experiment). so please, don't use this in
production machines (i'm pretty sure you knew that already :) and save all
data before trying.

>> technicalities

a jiffie is a kernel counter that among other things, counts the uptime.
it's incremented each 0.01 seconds on x86 and is held in a long variable, but
not "long" enough. :) it's unsigned, but you can use about -50000 to see a
glourious 497 days uptime in 32bit x86, which is the maximum (haven't tested
other archs, but a long in 64 bits archs is usually 64 bits thus allowing more
uptime). when it wraps up, you return to 0 uptime, just like after a reboot.
the system doesn't reboot though, no need to worry.

this module basically just changes the kernel jiffies. other aproaches are to
overwrite /proc/uptime but that requires to patch the sysinfo() system call
as well, and who-knows-what-else to make sure userland processes can't get
the real uptime (like nmap -O). on top of that, of course, the module must
stay loaded to maintain the fake proc entry and such. so in my opinion, the 
jiffies are the way to go. the downside is that the kernel behaves strangely
while changing jiffies, presumably because of timers gone crazy with the time
warp. side-effects include turning on the screen saver, increased time advance
(so you should set the time after using this) and tcp connections can go down
with big uptime changes, but not always. i'm sure other glitches exist,
but hey, you only need to run this once while booting. :)

>> moral issues

you are a _real_ lameass if you try to fool uptime contests or brag in general
about your l33t uptime. this is meant to be funny (in a good way), and to
remind me of lkm coding. :) anything else is just plain stupid. thank you.

*/

#define __KERNEL__
#define MODULE

        #if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
           #include <linux/modversions.h>
           #define MODVERSIONS
        #endif  

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/tqueue.h>
//#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tiago Sousa <mirage@kaotik.org>");
MODULE_DESCRIPTION("change system uptime");

static unsigned long int newjiffies = 0;
static unsigned long int newseconds = 0;
static unsigned int newdays = 0;
static unsigned char log = 1;
static unsigned int interruptnum = 50;
static unsigned long int jiffiesinc = 10000000;

MODULE_PARM (newjiffies, "l");
MODULE_PARM (newseconds, "l");
MODULE_PARM (newdays, "i");
MODULE_PARM (log, "b");
MODULE_PARM (interruptnum, "i");
MODULE_PARM (jiffiesinc, "l");

static DECLARE_WAIT_QUEUE_HEAD (WaitQ);
int cleaning_up = 0;
int time_to_inc = 0;
int status = 0;

static void intrpt_routine (void *);

static struct tq_struct task = { {NULL}, 0, intrpt_routine, NULL };

static
void intrpt_routine (void *irrelevant)
{
   if (cleaning_up && status == 2)		// if jiffies done
     wake_up (&WaitQ);				// cleanup so rmmod can return
   else
     queue_task (&task, &tq_timer);		// else back to the queue

   time_to_inc++;
   if (time_to_inc == interruptnum)
     {
       if (status == 0)				// jiffies > newjiffies
         {
           jiffies += jiffiesinc;
           if (jiffies < newjiffies)
             status++;
         }
       else if (status == 1)			// jiffies < newjiffies
         {
           if (newjiffies - jiffies < jiffiesinc)
             {
               jiffies += newjiffies - jiffies;
               status++;
             }
           else
             jiffies += jiffiesinc;
         }
       time_to_inc = 0;
     }
}

int
init_module (void)
{
  // calculate newjiffies
  if (newseconds != 0)
    newjiffies = newseconds * HZ;
  if (newdays != 0)
    newjiffies = newdays * 24 * 60 * 60 * HZ;

  // validate
  if (log)
    printk (KERN_ALERT "[myuptime] setting jiffies to %lu\n", newjiffies);
  if (newjiffies == -1)
    {
      if (log)
        printk (KERN_ALERT "[myuptime] value is too large\n");
      return 1;
    }
  if (newjiffies == 0)
    {
      if (log)
        {
          printk (KERN_ALERT "[myuptime] usage: insmod myuptime.o newdays=<days> && rmmod myuptime\n");
          printk (KERN_ALERT "[myuptime] read the source for more info\n");
        }
      return 1;
    }

  // setup timer
  status = (jiffies < newjiffies);
  queue_task (&task, &tq_timer);

  return 0;
}

void cleanup_module(void)
{
  // because we can't unload without removing the timer first
  cleaning_up = 1;
  sleep_on (&WaitQ);
  if (log)
    printk (KERN_ALERT "[myuptime] uptime change is complete\n");
}

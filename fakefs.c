/********************************************************/
/*                                                      */
/* by adventuresin9                                     */
/*                                                      */
/* A basic example of a Read Only sythetic file system. */
/* It place an entry in /srv, and mounts to /n.         */
/* The files it provides are examples of procedurally   */
/* generated content.                                   */
/*                                                      */
/* tyme uses time() to give a silly message             */
/* backtalk uses getuser() for a personal response      */
/* i-ching uses nrand() to generate hexagrams           */
/*                                                      */
/* Inspired by gpsfs                                    */
/* /sys/src/cmd/aux/gps                                 */
/********************************************************/


#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>


typedef struct Fakefile Fakefile;


/* This holds the information for the fake files */

struct Fakefile {
	char *name;
	char* (*fsread)(Req*);  /* this holds the function that will be called on a read */
	int mode;
};



void	fsopen(Req *r);
void	fsread(Req *r);
void	fswrite(Req *r);
void	fsend(Srv *s);
char*	timefunc(Req*);
char*	tbfunc(Req*);
char*	icfunc(Req*);


/* Here the fake files are specified               */
/* the name they will appear as in the file system */
/* the function that is called when they are read  */
/* and the permission mode they are given          */

Fakefile files[] = {
	{ "tyme",		timefunc,	0444 },
	{ "backtalk",	tbfunc,		0444 },
	{ "i-ching",	icfunc,		0444 },
};


Srv s = {
	.open	= fsopen,
	.read	= fsread,
	.write	= fswrite,
	.end	= fsend,
};

File *root;
File *fakedir;


void
fsend(Srv *)
{

	sysfatal("abandon ship");
}


void
fsopen(Req *r)
{

	respond(r, nil);
}


void
fsread(Req *r)
{
	Fakefile *f;

	r->ofcall.count = 0;
	f = r->fid->file->aux;
	respond(r, f->fsread(r));
}


void
fswrite(Req *r)
{

/* not used in this program */
/* just a place holder      */

	respond(r, nil);
}


void
fsinit(void)
{

/* This builds the fake directory    */
/* and fills it with the fake files  */
/* setting the owner and permissions */

	char *user;
	int i;

	user = getuser();
	s.tree = alloctree(user, user, 0555, nil);
	if(s.tree == nil)
		sysfatal("alloctree failed");
	root = s.tree->root;
	if((fakedir = createfile(root, "fake", user, DMDIR|0555, nil)) == nil)
		sysfatal("createfile failed");
	for(i = 0; i < nelem(files); i++)
		if(createfile(fakedir, files[i].name, user, files[i].mode, files + i) == nil)
			sysfatal("failed on %s", files[i].name);
}


void
threadmain(int argc, char *argv[])
{
	char *srvname, *mntpt;


/* I was lazy and didn't bother setting args for */
/* user selectable mount points                  */
/* Edit mntpt if you want the fake directory to  */
/* appear somewhere other than /n                */


	srvname = "fakefs";
	mntpt = "/n";

	fsinit();
	threadpostmountsrv(&s, srvname, mntpt, MBEFORE);
	threadexits(nil);
}




char*
timefunc(Req *r)
{
	char buf[512];

	snprint(buf, sizeof buf, "And it came to pass, in \n%ld seconds of\nthe reign of Unix...\n", time(0));

	readstr(r, buf);
	return nil;
}


char*
tbfunc(Req *r)
{
	char buf[512];

	snprint(buf, sizeof buf, "I'm sorry %s, I'm afraid I can't do that.\n", getuser());

	readstr(r, buf);
	return nil;
}


char*
icfunc(Req *r)
{

/* this loops 6 times, getting a number from 6 to 9 */
/* then sorts by even or odd, formats it            */
/* and appends it to the buffer                     */

	char *even = "___   ___";
	char *odd = "_________";
	char buf[512], *p;
	int i, d;

	p = buf;

	for (i = 0; i < 6; i++){
		d = 6 + nrand(4);
		if (d % 2)
			p = seprint(p, buf + sizeof buf, "%s %d\n", odd, d);
		else
			p = seprint(p, buf + sizeof buf, "%s %d\n", even, d);
	}
	
	readstr(r, buf);
	return nil;
}

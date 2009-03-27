#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "tran.h"

static hashtable h;
static hashtable vals;

void die(char *msg)
{
    fprintf(stderr, msg);
    exit(1);
}

int main(int argc, char **argv)
{
    int f;
    int len, page;
#define MAXLEN 128
    char key[MAXLEN], val[MAXLEN];
    hstr vh;
    
    if (argc!=2)
        die("Usage: makehash outfile\n");
    
    h=init_hash();
    vals=init_hash();
    
    while(fgets(key, MAXLEN, stdin))
    {
        if (!fgets(val, MAXLEN, stdin))
            die("Half of a key/value pair\n");
        *strchrnul(key, '\n')=0;
        *strchrnul(val, '\n')=0;
        if (!(vh=get_hash(vals, val)))
        {
            if ((vh=get_hash(h, key)) && vh!=-1)
                hfree(h, vh);
            else
                set_hash_nostring(h, key, -1);	// store the key first
            vh=hstore(h, val);
            set_hash_nostring(vals, val, vh);
        }
        set_hash_nostring(h, key, vh);
    }
    
    f=open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, 0666);
    if (!f)
        die("Can't open outfile\n");
    write(f, h, sizeof(int));
    write(f, h->tab, h->size*sizeof(struct hashentry));
    len = sizeof(int)+h->size*sizeof(struct hashentry);
    page = sysconf(_SC_PAGE_SIZE);
    lseek(f, page-len%page, SEEK_END);
    *((int*)h->hstart)=h->hcur-h->hstart;
    write(f, h->hstart, h->hcur-h->hstart);
    close(f);
    
    printf("%s: %i entries, %i bytes of string data.\n",
        argv[1], h->nval, h->hcur-h->hstart);
    
//    free_hash(h);
//    free_hash(vals);
    return 0;
}

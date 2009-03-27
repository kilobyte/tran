#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "tran.h"


#define crc(x) ((unsigned int)crc32(0, (unsigned char*)x, strlen(x)))
#define cmp(s,b) strcmp(h->hstart+s,b)

hstr hstore(hashtable h, char *x)
{
    int st, len = strlen(x);
    
    if (h->hcur+len>=h->hlimit)
        abort();
    strcpy(h->hcur, x);
    st = h->hcur-h->hstart;
    h->hcur += len+1;
    return st;
}

void hfree(hashtable h, hstr x)
{
    int len = strlen(h->hstart+x)+1;
    
    if (h->hstart+x != h->hcur-len)
        return; // Can't free except at the end.
    h->hcur-=len;
}


/**********************************/
/* initialize an empty hash table */
/**********************************/
hashtable init_hash()
{
    hashtable h=malloc(sizeof(struct hashtable));
    h->size=6;
    h->nent=0;
    h->nval=0;
    h->tab=calloc(h->size, sizeof(struct hashentry));

#define HSIZE 100*1048576
    h->hstart=malloc(HSIZE);
    if (!h->hstart)
        abort();
    h->hcur=h->hstart+sizeof(int);
    h->hlimit=h->hstart+HSIZE;
    
    return h;
}


/*********************/
/* free a hash table */
/*********************/
void free_hash(hashtable h)
{
    int i;
    
    if (h->nval)
        for (i=0; i<h->size; i++)
        {
            if (h->tab[i].left && (h->tab[i].left!=-1))
            {
                hfree(h, h->tab[i].left);
                hfree(h, h->tab[i].right);
            }
        };
    free(h->tab);
    free(h->hstart);
    free(h);
}


static void add_hash_value(hashtable h, hstr left, hstr right)
{
    int i;
    i=crc(h->hstart+left)%h->size;
    while (h->tab[i].left)
    {
        if (!i)
            i=h->size-1;
        i--;
    }
    h->tab[i].left = left;
    h->tab[i].right= right;
}



static void rehash(hashtable h, int s)
{
    int i,gs;
    struct hashentry *gt;
    
    gt=h->tab;
    gs=h->size;
    h->tab=calloc(s, sizeof(struct hashentry));
    h->nent=h->nval;
    h->size=s;
    for(i=0;i<gs;i++)
    {
        if (gt[i].left && gt[i].left!=-1)
            add_hash_value(h, gt[i].left, gt[i].right);
    }
    free(gt);
}



/********************************************************************/
/* add a (key,value) pair to the hash table, rehashing if necessary */
/********************************************************************/
void set_hash(hashtable h, char *key, char *value)
{
    int i,j;

    if (h->nent*5 > h->size*4)
        rehash(h, h->nval*3);
    j=-1;
    i=crc(key)%h->size;
    while (h->tab[i].left)
    {
        if (h->tab[i].left==-1)
            if (j==-1)
                j=i;
        if (!cmp(h->tab[i].left, key))
        {
            if (cmp(h->tab[i].right, value))
            {
                hfree(h, h->tab[i].right);
                h->tab[i].right=hstore(h, value);
            }
            return;
        }
        if (!i)
            i=h->size;
        i--;
    }
    if (j!=-1)
        i=j;
    else
        h->nent++;
    h->tab[i].left = hstore(h, key);
    h->tab[i].right= hstore(h, value);
    h->nval++;
}


void set_hash_nostring(hashtable h, char *key, hstr value)
{
    int i,j;

    if (h->nent*5 > h->size*4)
        rehash(h, h->nval*3);
    j=-1;
    i=crc(key)%h->size;
    while (h->tab[i].left)
    {
        if (h->tab[i].left==-1)
            if (j==-1)
                j=i;
        if (!cmp(h->tab[i].left, key))
        {
            h->tab[i].right=value;
            return;
        }
        if (!i)
            i=h->size;
        i--;
    }
    if (j!=-1)
        i=j;
    else
        h->nent++;
    h->tab[i].left = hstore(h, key);
    h->tab[i].right= value;
    h->nval++;
}



/****************************************************/
/* get the value for a given key, or 0 if not found */
/****************************************************/
hstr get_hash(hashtable h, char *key)
{
    int i;
    
    i=crc(key)%h->size;
    while (h->tab[i].left)
    {
        if (h->tab[i].left!=-1 && !cmp(h->tab[i].left, key))
        {
            return h->tab[i].right;
        }
        if (!i)
            i=h->size;
        i--;
    }
    return 0;
}



/****************************************************/
/* delete the key and its value from the hash table */
/****************************************************/
int delete_hash(hashtable h, char *key)
{
    int i;

    i=crc(key)%h->size;
    while (h->tab[i].left)
    {
        if (h->tab[i].left!=-1 && (!cmp(h->tab[i].left, key)))
        {
            hfree(h, h->tab[i].left);
            hfree(h, h->tab[i].right);
            h->tab[i].left=-1;
            h->nval--;
            if (h->nval*5+1<h->size)
                rehash(h, h->size/3);
            return 1;
        }
        if (!i)
            i=h->size;
        i--;
    }
    return 0;
}

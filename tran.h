/***************/
/* string heap */
/***************/
#define hstr int

/**************/
/* hash table */
/**************/
struct hashentry
{
    hstr left;
    hstr right;   
};

typedef struct hashtable
{
    int size;               /* allocated size */
    int nent;               /* current number of entries */
    int nval;               /* current number of values (entries-deleted) */
    struct hashentry *tab;  /* entries table */
    char *hstart;           /* heap of strings */
    char *hlimit;           /* limit of h */
    char *hcur;             /* pointer into the free part of h */
} *hashtable;

hashtable init_hash();
void free_hash(hashtable h);
void set_hash(hashtable h, char *key, char *value);
void set_hash_nostring(hashtable h, char *key, hstr value);
hstr get_hash(hashtable h, char *key);
int delete_hash(hashtable h, char *key);
hstr hstore(hashtable h, char *x);
void hfree(hashtable h, hstr x);

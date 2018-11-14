//list.c
#define NHASH			4
#define CACHENUM		12
#define NSTAT			6

#define STAT_LOCKED     0x1
#define STAT_VALID		0x2
#define STAT_DWR		0x4
#define STAT_KRDWR		0x8
#define STAT_WAITED		0x10
#define STAT_OLD		0x20

struct buf_header {
	int blkno;
	struct buf_header *hash_fp;
	struct buf_header *hash_bp;
	unsigned int stat;
	struct buf_header *free_fp;
	struct buf_header *free_bp;
	char *cache_data;
};

struct buf_header* hash_search(int);
int hash(int);
void insert_head_hash(struct buf_header*, struct buf_header*);
void insert_bottom_hash(struct buf_header*, struct buf_header*);
void insert_head_free(struct buf_header*, struct buf_header*);
void insert_bottom_free(struct buf_header*, struct buf_header*);
void remove_hash(struct buf_header*);
void remove_free(struct buf_header*);
void print_stat(struct buf_header*);

//command.c
void help_proc(int, char**);
void init_proc(int, char**);
void buf_proc(int, char**);
void hash_proc(int, char**);
void getblk_proc(int, char**);
void brelse_proc(int, char**);
void free_proc(int, char**);
void set_proc(int, char**);
void reset_proc(int, char**);
void quit_proc(int, char**);

//getblk.c
struct buf_header* getblk(int);

//brelse.c
void brelse(struct buf_header* buffer);

//global valiable
struct buf_header hash_head[NHASH];
struct buf_header free_head;
struct buf_header header_list[CACHENUM];
char init_flag;

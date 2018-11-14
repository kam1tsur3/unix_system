#include <stdio.h>
#include "main.h"

struct buf_header* getblk(int blkno)
{
	while(1){
		struct buf_header* tmp = hash_search(blkno);
		if(tmp != NULL){
			if((tmp->stat & STAT_LOCKED) != 0){
				//scenario5
				//sleep();
				printf("Scenario5\n");
				printf("Process goes to sleep.\n");
				tmp->stat |= STAT_WAITED;
				return NULL;
			}
			//scenario1
			remove_free(tmp);
			printf("Scenario1\n");
			return tmp;
		}else{
			tmp = free_head.free_fp;
			if(tmp == &free_head){
				//scenario4
				//sleep();
				printf("Scenario4\n");
				printf("Process goes to sleep.\n");
				return NULL;
			}
			remove_free(tmp);
			if(tmp->stat & STAT_DWR){
				//scenario3
				printf("Scenario3\n");
				tmp->stat = STAT_LOCKED | STAT_VALID | STAT_KRDWR | STAT_OLD;
				continue;
			}
			//scenario2
			printf("Scenario2\n");
			remove_hash(tmp);
			tmp->blkno = blkno;
			tmp->stat = STAT_KRDWR | STAT_LOCKED;
			insert_bottom_hash(&hash_head[hash(blkno)], tmp);
			tmp->stat = STAT_LOCKED | STAT_VALID;
			return tmp;
		}
	}
}




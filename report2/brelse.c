#include <stdio.h>
#include "main.h"

void brelse(struct buf_header *buffer)
{
	// wakeup();
	printf("Wakeup processes waiting for any buffer.\n");
	// wakeup();
	printf("Wakeup processes waiting for buffer of blkno %d.\n", buffer->blkno);
	// raise_cpu_level();
	if((buffer->stat & STAT_VALID) != 0 && (buffer->stat & STAT_OLD) == 0){
		insert_bottom_free(&free_head, buffer);
		printf("Insert the buffer at the bottom of freelist.\n");
	}
	else{
		insert_head_free(&free_head, buffer);
		printf("Insert the buffer at the bottom of freelist.\n");
	}
	// lower_cpu_level();	
	buffer->stat &= ~(STAT_LOCKED|STAT_WAITED|STAT_OLD);	
}

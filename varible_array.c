#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/can.h>

struct bcm_timeval {
	long tv_sec;
	long tv_usec;
};

struct VA {
	long int nbr_element;
	int elements[0];
};

struct bcm_msg_head {
	__u32 opcode;
	__u32 flags;
	__u32 count;
	struct bcm_timeval ival1, ival2;
	canid_t can_id;
	__u32 nframes;
};

int
main()
{
	printf("sizeof VA: %zd\n", sizeof(struct VA));
	printf("sizeof BCM: %zd\n", sizeof(struct bcm_msg_head));

	struct VA *va;
	size_t len = sizeof(struct VA) + 10 * sizeof *va->elements;
	printf("sizeof VA.elements %zd\n", sizeof *va->elements);
	printf("malloc(%zd)\n", len);
	va = malloc(len);
	free(va);
}

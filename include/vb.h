#ifndef __VB_H__
#define __VB_H__

/**
 * Utility functions
*/
static unsigned long vb_set(unsigned long v, unsigned pos) {
	return v |= (1 << pos);
}

/*static unsigned long vb_clr(unsigned long v, unsigned pos) {
	return v &= ~(1 << pos);
}*/

/*unsigned ctz(unsigned long v) {
	unsigned count = 0;
	for (; v; v >>= 1, ++count) {}
	return count;
}*/

#endif

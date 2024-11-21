#include <stdio.h>

void secret_func(void)
{
	puts("Channel open.");
}
// addr de ret: 0x4011cf
void visible_func(void)
{
	unsigned int s = 0x42424242;
	// rbp            0x7fffffffdc80      0x7fffffffdc80
	// rsp            0x7fffffffdc30      0x7fffffffdc30
	char buffer[64];

	printf("Please enter your message: ");
	fgets(buffer, 128, stdin);

	if (s == 0x5a5a5a5a)
		puts("Comm-link online.");
}
// #1  0x0000000000401175 in visible_func () at bo_practice.c:13
// #2  0x00000000004011cc in main () at bo_practice.c:24
int main(void)
{
	puts("Go ahead, TACCOM");
	visible_func();
	return 0;
}
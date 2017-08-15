#include <stdio.h>
int main()
{
	char text[1024000];
	int i=0;
	FILE * fp=fopen("myfsys", "r");
	if(fp==NULL)
		printf("open failed.");
	else
	{
		printf("open succeed.");
		fread(text, 1024, 1000, fp);
		for(i=0; i<1024000; i++)
		{
			if(text[i]=='a')
				printf("%d\n", i);
		}

		fclose(fp);
	}
}

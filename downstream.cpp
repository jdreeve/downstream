#include <stdio.h>

void parse_data_file(void);

int main(int argc, char* argv[]){

	parse_data_file();
	printf("Main exiting\n");
}

void parse_data_file(void){
	FILE* data_file = fopen("data.txt", "r");
	char buf[300];

	while(fgets(buf, sizeof(buf), data_file) != NULL){
		printf("%s\n", buf);
	}
}

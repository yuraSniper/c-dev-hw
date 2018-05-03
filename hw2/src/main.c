#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define RD_STR_INIT_BUF_SIZE 8

char * read_string(uint32_t * str_length)
{
	//TODO: We probably should check that malloc didn't return NULL
	char * result_str = malloc(sizeof(char) * RD_STR_INIT_BUF_SIZE);
	int32_t capacity = RD_STR_INIT_BUF_SIZE;
	int32_t length = 0;

	char ch;
	do
	{
		ch = getchar();

		if (length + 1 == capacity)
		{
			//TODO: We probably should check that malloc didn't return NULL
			result_str = realloc(result_str, capacity * 2);
			capacity *= 2;
		}

		//NOTE(yura): in 0-based indexing, length is basically the index of the next element
		result_str[length] = ch;
		length++;
	}
	while(ch != '\n');

	result_str[length] = '\0';

	if (str_length != NULL)
		*str_lenght = length;

	return result_str;
}

//IMPORTANT: This function is unsafe! 
//Make sure that dst buffer's size is >= than the length of src string
char * my_strcpy(char * dst, const char * src)
{
	while (*src != '\0')
	{
		*dst = *src;
		dst++;
		src++;
	}

	return dst;
}

char * process_strings(char * str1, uint32_t str1_length, char * str2, uint32_t str2_length)
{
	bool char_present[256];

	char * str1_cursor = str1;

	while (*str1_cursor != NULL)
	{
		char_present[(uint8_t) *str1_cursor] = true;
		str1_cursor++;
	}

	//NOTE: This string is potentially bigger than necessary, because calculating
	//the size would require a call to strlen and that would be slower
	//TODO: We probably should check that malloc didn't return NULL
	char * result_str = malloc(sizeof(char) * (str1_length + str2_length));

	char * result_cursor = my_strcpy(result_str, str1);
	char * str2_cursor = str2;

	while (*str2_cursor != '\0')
	{
		if (!char_present[(uint8_t) *str2_cursor])
		{
			*result_cursor = *str2_cursor;
			
		}
	}
}

int main(int argc, char * argv[])
{
	printf("Enter first string: ");

	uint32_t str1_length = 0;
	char * str1 = read_string(&str1_length);

	printf("Enter second string: ");

	uint32_t str2_length = 0;
	char * str2 = read_string(&str2_length);

	char * result_str = process_strings(str1, str2);

	printf("Result: %s\n", result_str);
}

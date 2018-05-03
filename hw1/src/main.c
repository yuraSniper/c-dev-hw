#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define RD_STR_INIT_BUF_SIZE 8

char * read_string()
{
	char * result_str = malloc(sizeof(char) * RD_STR_INIT_BUF_SIZE);
	int32_t capacity = RD_STR_INIT_BUF_SIZE;
	int32_t length = 0;

	char ch;
	do
	{
		ch = getchar();

		if (length + 1 == capacity)
		{
			result_str = realloc(result_str, capacity * 2);
			capacity *= 2;
		}

		//NOTE(yura): in 0-based indexing, length is basically the index of the next element
		result_str[length] = ch;
		length++;
	}
	while(ch != '\n');

	result_str[length] = '\0';

	return result_str;
}

bool my_isalnum(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

char * reverse_word(char * str)
{
	if (str == NULL || *str == '\0')
		return NULL;

	char * word_start = str;

	while (*word_start != '\0' && !my_isalnum(*word_start))
		word_start++;

	char * word_end = word_start;

	while (*word_end != '\0' && my_isalnum(*word_end))
		word_end++;

	int32_t word_length = (word_end - word_start);

	for (int32_t index = 0; index < word_length / 2; index++)
	{
		char tmp = word_start[index];
		word_start[index] = word_end[-index - 1];
		word_end[-index - 1] = tmp;

		//NOTE(yura): I know that it could be done with
		//word_start[word_length - index - 1]
		//instead of
		//word_end[-index - 1]
		//but i thought it would be more interesting
		//to address off of a pointer in backwards direction :P
	}

	return word_end;
}

char * reverse_words(char * str)
{
	char * word_cursor = str;

	while (word_cursor != NULL)
		word_cursor = reverse_word(word_cursor);

	return str;
}

int main(int argc, char * argv[])
{
	printf("Enter string: ");

	char * str = read_string();

	str = reverse_words(str);

	printf("Result: %s\n", str);
}

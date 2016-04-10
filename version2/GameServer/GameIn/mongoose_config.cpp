#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "mongoose_config.h"
#include "mongoose.h"

#define MAX_CONF_FILE_LINE_SIZE 1024

char* sdup(const char *str)
{
	const int size = (int)strlen(str) + 1;
	char* p = (char*)malloc(size);
	if (p) strcpy_s(p, size, str);
	return p;
}

void set_option(char **options, const char *name, const char *value)
{
	int i;
	for (i = 0; i < MG_MAX_OPTIONS - 3; ++i)
	{
		if (options[i] == NULL)
		{
			options[i] = sdup(name);
			options[i + 1] = sdup(value);
			options[i + 2] = NULL;
			break;
		}
	}

	if (i == (MG_MAX_OPTIONS - 3))
		printf("error : Too many options specified\n");
}

bool mg_load_command_line_arguments(char **options)
{
	options[0] = NULL;

	// load config file settings first
	FILE *fp = NULL;
	if (fopen_s(&fp, MG_CONFIG_FILE, "r") == 0)
	{
		// loop over the lines in config file
		int line_no = 0;
		char line[MAX_CONF_FILE_LINE_SIZE] = { 0 };
		while (fgets(line, sizeof(line), fp))
		{
			line_no++;

			// ignore empty lines and comments
			size_t i;
			for (i = 0; isspace(*(unsigned char *)&line[i]);) ++i;
			if (line[i] == '#' || line[i] == '\0') continue;

			char opt[256] = { 0 };
			char val[256] = { 0 };
			if (sscanf_s(line, "%s %[^\r\n#]", opt, sizeof(opt), val, sizeof(val)) != 2)
				printf("%s: line %d is invalid, ignoring it:\n %s", MG_CONFIG_FILE, (int)line_no, line);
			else
				set_option(options, opt, val);
		}

		fclose(fp);
	}
	else return false;
	return true;
}

bool mg_create_config_file(const char *path)
{
	const char *config_file_top_comment = "# Mongoose web server configuration file.\n";

	// create configure file if it is not present yet
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "a+") == 0)
	{
		fprintf(fp, "%s", config_file_top_comment);
		const char **names = mg_get_valid_option_names();

		struct mg_callbacks callbacks;
		memset(&callbacks, 0, sizeof(callbacks));
		mg_context* ctx = mg_start(&callbacks, NULL, NULL);
		for (int i = 0; names[i * 2]; ++i)
		{
			const char * value = mg_get_option(ctx, names[i * 2]);
			fprintf(fp, (value && *value != 0) ? "%s %s\n" : "# %s %s\n", names[i * 2], value ? value : "<value>");
		}
		mg_stop(ctx);
		fclose(fp);
	}
	else return false;
	return true;
}

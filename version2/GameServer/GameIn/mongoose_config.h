#ifndef GUARD_mongoose_config_HEADER_FILE
#define GUARD_mongoose_config_HEADER_FILE

#define MG_MAX_OPTIONS		40
#define MG_CONFIG_FILE		"gamein.config"

bool mg_load_command_line_arguments(char **options);

bool mg_create_config_file(const char *path);

#endif	//	GUARD_mongoose_config_HEADER_FILE



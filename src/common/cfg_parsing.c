#include <stdio.h>

#include "cfg_parsing.h"
#include "mem_management.h"
#include "log_handler.h"


int read_cfg(char **ret_client_id, char** ret_address, long int* ret_comm_port)
{
    /*
    ** reads connection configuration from CFG file
    */
    char *address, *comm_port, *client_id;
    FILE* cfg_file;
    
    cfg_file = fopen(CFG_FILE, "r");
    if (cfg_file == NULL) {
        rds_log(logDEBUG, "Could not open cfg file, using defaults.\n");

        *ret_address = (char*)rds_malloc(10*sizeof(char));
        strncpy(*ret_address, "127.0.0.1\0", 10);

        *ret_comm_port = 2345;
        return 1;
    }

    rds_log(logDEBUG, "Parsing the config file.\n");

    char fmstate = 'e';
    char act_c;

    int address_parsed = 0;
    int client_id_parsed = 0;
    int comm_port_parsed = 0;

    do {
        act_c = fgetc(cfg_file);
        switch (fmstate) {
            case 'e':
                if ((act_c == '#') || (act_c == '[')) //comment
                    fmstate = 'c';
                else if (act_c == 'a') //address
                    fmstate = 'a';
                else if (act_c == 's') //address (alternative)
                    fmstate = 's';
                else if (act_c == 'p') //port
                    fmstate = 'p';
                else if (act_c == 'i') //id
                    fmstate = 'i';
                else fmstate = 'c'; //ignore invalid lines
            break;

            case 'c':
                if (act_c == '\n')
                    fmstate = 'e';
            break;

            case 'a':
                compare(cfg_file, "address=", 7, &fmstate, &act_c);
                
                if (fmstate == 'a') {
                    address = file_read_value(cfg_file, 0);
                    *ret_address = address;
                    address_parsed = 1;
                    fmstate = 'e';
                }
            break;

            case 's':
                compare(cfg_file, "server=", 6, &fmstate, &act_c);
                
                if (fmstate == 's') {
                    address = file_read_value(cfg_file, 0);
                    *ret_address = address;
                    address_parsed = 1;
                    fmstate = 'e';
                }
            break;

            case 'p':
                compare(cfg_file, "port=", 4, &fmstate, &act_c);
                
                if (fmstate == 'p') {
                    comm_port = file_read_value(cfg_file, 5);
                    *ret_comm_port = strtol(comm_port, NULL, 10);
                    rds_free(comm_port);
                    comm_port_parsed = 1;
                    fmstate = 'e';
                }
            break;

            case 'i':
                compare(cfg_file, "id=", 2, &fmstate, &act_c);
                
                if (fmstate == 'i') {
                    client_id = file_read_value(cfg_file, 0);
                    *ret_client_id = client_id;
                    client_id_parsed = 1;
                    fmstate = 'e';
                }
            break;

            default:
            break;
        }
    }
    while (act_c != EOF);

    fclose(cfg_file);

    if (address_parsed == 0) {
        *ret_address = (char*)rds_malloc(10*sizeof(char));
        strncpy(*ret_address, "127.0.0.1\0", 10);
    }
    if (client_id_parsed == 0)
        *ret_client_id = NULL; // client do not have an Id yet
    if (!comm_port_parsed)
        *ret_comm_port = 2345;

    rds_log(logDEBUG, "server: %s, comm port: %ld\n", *ret_address, *ret_comm_port);

    return 0;
}

int read_srv_cfg(long int* ret_comm_port)
{
    /*
    * reads connection configuration from CFG file
    */

    char *comm_port;
    FILE* cfg_file;

    cfg_file = fopen(CFG_FILE, "r");
    if (cfg_file == NULL) {
        rds_log(logDEBUG, "Could not open cfg file, using defaults.\n");

        *ret_comm_port = 2345;
        return 1;
    }

    rds_log(logDEBUG, "CFG file opened.\n");

    char fmstate = 'e';
    char act_c;

    int comm_port_parsed = 0;

    do {
        act_c = fgetc(cfg_file);
        switch (fmstate) {
            case 'e':
                if (act_c == '#') //comment
                    fmstate = 'c';
                else if (act_c == 'p') //port
                    fmstate = 'p';
                else fmstate = 'c'; //ignore invalid lines
            break;

            case 'c':
                if (act_c == '\n')
                    fmstate = 'e';
            break;

            case 'p':
                compare(cfg_file, "port=", 4, &fmstate, &act_c);
                
                if (fmstate == 'p') {
                    comm_port = file_read_value(cfg_file, 5);
                    *ret_comm_port = strtol(comm_port, NULL, 10);
                    rds_free(comm_port);
                    comm_port_parsed = 1;
                    fmstate = 'e';
                }
            break;

            default:
            break;
        }
    }
    while (act_c != EOF);

    fclose(cfg_file);

    if (!comm_port_parsed)
        *ret_comm_port = 2345;

    rds_log(logDEBUG, "comm port: %ld\n", *ret_comm_port);

    return 0;
}

int write_to_cfg(char *name, char *value)
{
    FILE* cfg_file;
    
    cfg_file = fopen(CFG_FILE, "r+");
    if (cfg_file == NULL) {
        rds_log(logDEBUG, "Conf file not found.\n");
        return 1;
    }

    rds_log(logDEBUG, "CFG file opened for write.\n");

    char fmstate = 'e';
    char *act_val;
    int already_exists = 0;

    int len = 0;
    while (name[len] != '\0')
        len++;

    int val_len = 0;
    while (value[val_len] != '\0')
        val_len++;

    char act_c;
    int position;

    do {
        act_c = fgetc(cfg_file);
        switch (fmstate) {
            case 'e':
                if (act_c == name[0]) //desired value
                    fmstate = 'v';
                else fmstate = 'c'; //ignore invalid lines
            break;

            case 'c':
                if (act_c == '\n')
                    fmstate = 'e';
            break;

            case 'v':
                compare(cfg_file, name, len - 1, &fmstate, &act_c);
                act_c = fgetc(cfg_file);
                if (act_c != '=')
                    fmstate = 'c';
                if (fmstate == 'v') {
                    already_exists = 1;
                    fmstate = 'e';
                    position = ftell(cfg_file);
                }
            break;

            default:
            break;
        }
    }
    while (act_c != EOF);

    if (already_exists == 1) {
    //clone file with changed value
        char *tempcfg;
        FILE *new_file;
        
        fseek(cfg_file, 0, SEEK_SET);
        tempcfg = (void*)rds_malloc((position + 1) * sizeof(char));
        fread(tempcfg, sizeof(char), position, cfg_file);
        tempcfg[position] = '\0';
        
        new_file= fopen("../etc/rds-client.conf.tmp", "w");
        fwrite(tempcfg, sizeof(char), position, new_file);
        fwrite(value, sizeof(char), val_len, new_file);


        do {
            act_c = fgetc(cfg_file);
        }
        while ((act_c != '\n') && (act_c != EOF));
        
        while (act_c != EOF) {
            fputc(act_c, new_file);
            act_c = fgetc(cfg_file);
        }

        fclose(cfg_file);
        fclose(new_file);

        if (remove(CFG_FILE) != 0) {
            rds_log(logERROR, "cannot delete old conf. file\n");
            return 1;
        }

        if (rename("../etc/ssds-client.conf.tmp", CFG_FILE) != 0) {
            rds_log(logERROR, "cannot rename new conf. file\n");
            return 1;
        }

        rds_log(logDEBUG, "there is already [%s] in config file, updating its value\n", name);
        return 0;
    }
    else {
        fseek(cfg_file, 0, SEEK_END);
        fwrite(name, sizeof(char), len, cfg_file);
        fwrite("=", sizeof(char), 1, cfg_file);
        fwrite(value, sizeof(char), val_len, cfg_file);
        fwrite("\n", sizeof(char), 1, cfg_file);

        rds_log(logDEBUG, "[%s = %s] written to configuration file\n", name, value);

        fclose(cfg_file);
    }
    return 0;
}

char* file_read_value(FILE* file, int max_length)
{
    /*
    ** reads value from file until EOL
    */
    char *value;
    char act_char;
    int value_lenght = 0;
    int allocated_lenght = 5;
    
    value = (char*)rds_malloc(6*sizeof(char)); //5 + 1
    act_char = fgetc(file);
    
    while ((act_char != '\n') && (act_char != EOF)) {
        if (value_lenght == allocated_lenght) {
            allocated_lenght += 5;
            value = (char*)rds_realloc(value, (allocated_lenght + 1)*sizeof(char));
        }
        value[value_lenght++] = act_char;
        act_char = fgetc(file);
    }
    
    value[value_lenght] = '\0';
    if ((value_lenght >= max_length) & (max_length != 0)) {
        rds_log(logDEBUG, "[%s] shortened to:\n", value);
        value[max_length - 1] = '\0';
        value = (char*)rds_realloc(value, (max_length)*sizeof(char));
        rds_log(logDEBUG, "\t[%s] (max-length = %d)\n", value, max_length);
    }
    //rds_log(logDEBUG, "***read \"%s\" of lenght %d\n", value, value_lenght);
    return value;
}


void compare(FILE* file, char* str, int len, char *state, char *act_c)
{
    for (int i1 = 1; i1 <= len; i1++) {
        if (*act_c != str[i1])
            *state = 'c';
        if (i1 < len)
            *act_c = fgetc(file);
    }
}

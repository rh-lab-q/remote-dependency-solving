
#include "cfg_parsing.h"

int read_cfg(char** ret_address, char** ret_comm_port, char** ret_data_port)
{
  /*
  ** reads connection configuration from CFG file
  */
  char *address, *comm_port, *data_port;
  FILE* cfg_file;
  cfg_file = fopen(CFG_FILE, "r");
  if (cfg_file == NULL)
  {
    ssds_log(logDEBUG, "Could not open cfg file, using defaults.\n");
    address = (char*)malloc(10*sizeof(char*));
    strcpy(address, "127.0.0.1");
    *ret_address = address;
    //comm_port = (char*)malloc(5*sizeof(char*));
    strcpy(ret_comm_port, "2345");
    //data_port = (char*)malloc(5*sizeof(char*));
    strcpy(ret_data_port, "2346");
    return 1;
  }
  ssds_log(logDEBUG, "CFG file opened.\n");

  char fmstate = 'e';
  char act_c;

  char* address_sstr = "address=";
  char* comm_port_sstr = "port=";
  char* data_port_sstr = "data-port=";

  int address_parsed = 0;
  int comm_port_parsed = 0;
  int data_port_parsed = 0;

  do
  {
    act_c = fgetc(cfg_file);
    switch (fmstate)
    {
      case 'e':
        if (act_c == '#') //comment
          fmstate = 'c';
        else if (act_c == 'a') //adress
          fmstate = 'a';
        else if (act_c == 'p') //port
          fmstate = 'p';
        else if (act_c == 'd') //data-port
          fmstate = 'd';
        else fmstate = 'c'; //ignore invalid lines
      break;

      case 'c':
        if (act_c == '\n')
          fmstate = 'e';
      break;

      case 'a':
        for (int i1 = 2; i1 <= 7; i1++)
        {
          act_c = fgetc(cfg_file);
          if (act_c != address_sstr[i1])
          {
            fmstate = 'c';
            break;
          }
          else
          {
          }
        }
        if (fmstate == 'a')
        {
          address = file_read_value(cfg_file, 255);
          address_parsed = 1;
          fmstate = 'e';
        }
      break;

      case 'p':
        for (int i1 = 2; i1 <= 4; i1++)
        {
          act_c = fgetc(cfg_file);
          if (act_c != comm_port_sstr[i1])
          {
            fmstate = 'c';
            break;
          }
        }
        if (fmstate == 'p')
        {
          comm_port = file_read_value(cfg_file, 5);
          strcpy(ret_comm_port, comm_port);
          free(comm_port);
          comm_port_parsed = 1;
          fmstate = 'e';
        }
      break;

      case 'd':
        for (int i1 = 2; i1 <= 9; i1++)
        {
          act_c = fgetc(cfg_file);
          if (act_c != data_port_sstr[i1])
          {
            fmstate = 'c';
            break;
          }
        }
        if (fmstate == 'd')
        {
          data_port = file_read_value(cfg_file, 5);
          strcpy(ret_data_port, comm_port);
          free(data_port);
          data_port_parsed = 1;
          fmstate = 'e';
        }
      break;

      default:
      break;
    }
  }
  while (act_c != EOF);

  fclose(cfg_file);

  if (address_parsed == 0)
  {
    address = (char*)ssds_malloc(10*sizeof(char*));
    strcpy(address, "127.0.0.1");
  }
  if (!comm_port_parsed)
  {
    comm_port = (char*)ssds_malloc(5*sizeof(char*));
    strcpy(*ret_comm_port, "2345");
  }
  if (!data_port_parsed)
  {
    data_port = (char*)ssds_malloc(5*sizeof(char*));
    strcpy(*ret_data_port, "2346");
  }

  ssds_log(logDEBUG, "server: %s, comm port: %s, data port: %s\n", address, ret_comm_port, ret_data_port);

  *ret_address = address;
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
  value = (char*)malloc(6*sizeof(char)); //5 + 1

  act_char = fgetc(file);
  while ((act_char != '\n') && (act_char != EOF))
  {
    if (value_lenght == allocated_lenght)
    {
      allocated_lenght += 5;
      value = (char*)realloc(value, (allocated_lenght + 1)*sizeof(char));
    }
    value[value_lenght++] = act_char;
    act_char = fgetc(file);
  }
  value[value_lenght] = '\0';
  if (value_lenght >= max_length)
  {
    ssds_log(logDEBUG, "[%s] shortened to:\n", value);
    value[max_length - 1] = '\0';
    value = (char*)realloc(value, (max_length)*sizeof(char));
    ssds_log(logDEBUG, "\t[%s] (max-length = %d)\n", value, max_length);
  }
  //ssds_log(logDEBUG, "***read \"%s\" of lenght %d\n", value, value_lenght);
  return value;
}
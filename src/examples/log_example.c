#include <stdio.h>
#include <stdlib.h>
#include <glib-2.0/glib.h>

static const gchar *log_level_to_string (GLogLevelFlags level)
{
  switch (level)
    {
      case G_LOG_LEVEL_ERROR:    return "ERROR";
      case G_LOG_LEVEL_CRITICAL: return "CRITICAL";
      case G_LOG_LEVEL_WARNING:  return "WARNING";
      case G_LOG_LEVEL_MESSAGE:  return "MESSAGE";
      case G_LOG_LEVEL_INFO:     return "INFO";
      case G_LOG_LEVEL_DEBUG:    return "DEBUG";
      default: return "UNKNOWN";
    }
}

static void ssds_log(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data )
{
  if (log_level <= G_LOG_LEVEL_WARNING)
  {
    g_printerr ("%s: %s: %s\n", log_domain, log_level_to_string(log_level), message);
  }
  else
  {
    g_print ("%s: %s: %s\n", log_domain, log_level_to_string(log_level), message);
  }
}

GPrintFunc ssds_err_print(const gchar* message)
{
  printf("tohle je error: %s\n", message);
}

void main()
{
  g_setenv("G_MESSAGES_DEBUG", "all", 1);
  g_setenv("G_MESSAGES_PREFIXED", "", 1);
  g_set_print_handler((GPrintFunc)ssds_err_print);
  g_log_set_handler("SSDS", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, ssds_log, NULL);
  
  
  
  g_warning("This is warning\n");
  g_message("This is message\n");
  g_debug("This is debug\n");
  g_critical("This is critical\n");
  g_info("info\n");
  
  g_print("bla bla\n");
}
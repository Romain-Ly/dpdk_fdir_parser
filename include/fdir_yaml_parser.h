#include <stdio.h>
#include <yaml.h>

#include <rte_ether.h>
#include <rte_eth_ctrl.h>
#include <rte_log.h>

enum fdir_parsing_cmd{
  ADD,
  UPDATE, /* NOT IMPLEMENT yet */
  DELETE /* NOT IMPLEMENT yet */
};
struct fdir_parsing {
  int port;
  enum fdir_parsing_cmd cmd;
  struct rte_eth_fdir_filter filter; 
};

#define FDIR_MAX_RULES 256
#define FDIR_NRULES_INIT 128



/**
 * functions
 **/


struct fdir_parsing* 
yaml_parser(char* file_name, struct fdir_parsing *filter_tmp,
	    int (*callback_func)(struct fdir_parsing*));

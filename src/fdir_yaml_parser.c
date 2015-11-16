#include "fdir_yaml_parser.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef UNIT_MAIN
#include <unistd.h>
#endif

//static struct rte_eth_fdir_filter fdir_filter_parsed;




#define BADKEY -1

typedef struct { const char *key; const int value; } symstruct_t;
/**
 * depth 0
 **/
#define FD_ADD 1
#define FD_DELETE 2
#define FD_UPDATE 3
static symstruct_t lkt_0[] = {
  { "add", FD_ADD }, 
  { "delete", FD_DELETE }, 
  { "update", FD_UPDATE },
};
#define LKT0_NKEYS (sizeof(lkt_0)/sizeof(symstruct_t))


/**
 * depth 1
 **/
#define FD_SOFT_ID 1
#define FD_PORT 2
#define FD_INPUT 3
#define FD_ACTION 4
static symstruct_t lkt_1[] = {
  { "soft_id", FD_SOFT_ID }, 
  { "port", FD_PORT }, 
  { "input", FD_INPUT }, 
  { "action", FD_ACTION },
};
#define LKT1_NKEYS (sizeof(lkt_1)/sizeof(symstruct_t))


/**
 * depth 2
 **/
/* input */
#define FD_FLOW_TYPE 1
#define FD_FLOW 2
#define FD_FLOW_EXT 3

/* action */
#define FD_ACT_RX_QUEUE 4
#define FD_ACT_BEHAVIOR 5
#define FD_ACT_REPORT_STATUS 6
#define FD_ACT_FLEX_OFF 7

static symstruct_t lkt_2[] = {
  { "flow_type", FD_FLOW_TYPE }, 
  { "flow", FD_FLOW }, 
  { "flow_ext", FD_FLOW_EXT },
  { "rx_queue",FD_ACT_RX_QUEUE },
  { "behavior", FD_ACT_BEHAVIOR },
  {"report_status", FD_ACT_REPORT_STATUS },
  {"flex_off",FD_ACT_FLEX_OFF},
};

#define LKT2_NKEYS (sizeof(lkt_2)/sizeof(symstruct_t))


/**
 * depth 3
 **/
/* flow_ext */
#define FD_VLAN_TCI 1
#define FD_FLEXBYTES 2

/* ip4_flow */
#define FD_IP4_FLOW 3


static symstruct_t lkt_3[] = {
  {"vlan_tci",FD_VLAN_TCI},
  {"flexbytes",FD_FLEXBYTES},
  {"ip4_flow",FD_IP4_FLOW},
};

#define LKT3_NKEYS (sizeof(lkt_3)/sizeof(symstruct_t))


/**
 * depth 4
 **/
/* ip4 flow */
#define FD_IP4_SRC_IP 1
#define FD_IP4_DST_IP 2


static symstruct_t lkt_4[] = {
  {"ip4_src_ip",FD_IP4_SRC_IP},
  {"ip4_dst_ip",FD_IP4_DST_IP},
};

#define LKT4_NKEYS (sizeof(lkt_4)/sizeof(symstruct_t))



/**
 * enum converters
 **/
/* flow type */
static symstruct_t lkt_fdir_flow_type[] = {
  {"RTE_ETH_FLOW_UNKNOWN",RTE_ETH_FLOW_UNKNOWN},
  {"RTE_ETH_FLOW_RAW",RTE_ETH_FLOW_RAW},
  {"RTE_ETH_FLOW_IPV4",RTE_ETH_FLOW_IPV4},
  {"RTE_ETH_FLOW_FRAG_IPV4",RTE_ETH_FLOW_FRAG_IPV4},
  {"RTE_ETH_FLOW_NONFRAG_IPV4_TCP",RTE_ETH_FLOW_NONFRAG_IPV4_TCP},
  {"RTE_ETH_FLOW_NONFRAG_IPV4_UDP",RTE_ETH_FLOW_NONFRAG_IPV4_UDP},
  {"RTE_ETH_FLOW_NONFRAG_IPV4_SCTP",RTE_ETH_FLOW_NONFRAG_IPV4_SCTP},
  {"RTE_ETH_FLOW_NONFRAG_IPV4_OTHER",RTE_ETH_FLOW_NONFRAG_IPV4_OTHER},
  {"RTE_ETH_FLOW_IPV6",RTE_ETH_FLOW_IPV6},
  {"RTE_ETH_FLOW_FRAG_IPV6",RTE_ETH_FLOW_FRAG_IPV6},
  {"RTE_ETH_FLOW_NONFRAG_IPV6_TCP",RTE_ETH_FLOW_NONFRAG_IPV6_TCP},
  {"RTE_ETH_FLOW_NONFRAG_IPV6_UDP",RTE_ETH_FLOW_NONFRAG_IPV6_UDP},
  {"RTE_ETH_FLOW_NONFRAG_IPV6_SCTP",RTE_ETH_FLOW_NONFRAG_IPV6_SCTP},
  {"RTE_ETH_FLOW_NONFRAG_IPV6_OTHER",RTE_ETH_FLOW_NONFRAG_IPV6_OTHER},
  {"RTE_ETH_FLOW_L2_PAYLOAD",RTE_ETH_FLOW_L2_PAYLOAD},
  {"RTE_ETH_FLOW_IPV6_EX",RTE_ETH_FLOW_IPV6_EX},
  {"RTE_ETH_FLOW_IPV6_TCP_EX",RTE_ETH_FLOW_IPV6_TCP_EX},
  {"RTE_ETH_FLOW_IPV6_UDP_EX",RTE_ETH_FLOW_IPV6_UDP_EX},
  {"RTE_ETH_FLOW_MAX",RTE_ETH_FLOW_MAX},
};
#define LKT_FDIR_FLOW_TYPE_NKEYS (sizeof(lkt_fdir_flow_type)/sizeof(symstruct_t))


/* fdir behavior */
static symstruct_t lkt_fdir_behavior[] = {
  {"RTE_ETH_FDIR_ACCEPT",RTE_ETH_FDIR_ACCEPT},
  {"RTE_ETH_FDIR_REJECT",RTE_ETH_FDIR_REJECT},
};
#define LKT_FDIR_BEHAVIOR_NKEYS (sizeof(lkt_fdir_behavior)/sizeof(symstruct_t))

/* report_status */
static symstruct_t lkt_fdir_status[] = {
  /**< Report nothing. */
  {"RTE_ETH_FDIR_NO_REPORT_STATUS",RTE_ETH_FDIR_NO_REPORT_STATUS},
  {"RTE_ETH_FDIR_REPORT_ID", RTE_ETH_FDIR_REPORT_ID}, /* < Only report FD ID. */
  /* < Report FD ID and 4 flex bytes. */
  {"RTE_ETH_FDIR_REPORT_ID_FLEX_4",RTE_ETH_FDIR_REPORT_ID_FLEX_4},
  {"RTE_ETH_FDIR_REPORT_FLEX_8",RTE_ETH_FDIR_REPORT_FLEX_8},
};

#define LKT_FDIR_STATUS_NKEYS (sizeof(lkt_fdir_status)/sizeof(symstruct_t))






/****************/
static int 
keyfromstring(char *key, symstruct_t *lookuptable, int nkeys){
  
  int i;
  for (i=0; i < nkeys; i++) {
    symstruct_t *sym = (lookuptable + i);
    if (strcmp( sym->key, key) == 0){
      return sym->value;
    }
  }
  return BADKEY;
}

static void
yaml_error( char* value, int depth){
  fprintf(stderr,"Got value '%s' in depth '%d' but cannot parse it. "
	  "Probably not implemented yet or BADKEY value, sorry.\n",value,depth);
  exit(EXIT_FAILURE);
}



static yaml_char_t *
yaml_get_next_scalar(yaml_parser_t *parser, yaml_event_t *event){
  
  /* delete old event */
  if(event->type != YAML_STREAM_END_EVENT){
      yaml_event_delete(event);
    } 
  
  if (!yaml_parser_parse(parser, event)) {
    printf("Parser error %d\n", parser->error);
    exit(EXIT_FAILURE);
  }
  switch(event->type) {

  case YAML_SCALAR_EVENT:
    
    return event->data.scalar.value;
  default :
    fprintf(stderr,"Parse error, no value\n");
    if(event->type != YAML_STREAM_END_EVENT){
      yaml_event_delete(event);
    }
    exit(EXIT_FAILURE);
  }

  
}




static int
yaml_update(int depth,struct fdir_parsing* filter_tmp, char* value,
	    yaml_parser_t *parser, yaml_event_t *event){
  
  struct in_addr *inp;
  inp = malloc(sizeof(struct in_addr));
    
  /**
   * struct in_addr {
   *  unsigned long s_addr;  
   * };
   **/
  
  switch(depth){

    /**
     * depth 0
     **/
  case 0:
    switch(keyfromstring(value,lkt_0,LKT0_NKEYS)) {
    case FD_ADD:
      filter_tmp->cmd = ADD ;
      break;
    case FD_DELETE:
    case FD_UPDATE:
    default:
      yaml_error(value,depth);

    }//end switch case 0
    break;//case 0

    /**
     * depth 1
     **/
  case 1:
    switch(keyfromstring(value,lkt_1,LKT1_NKEYS)) {
    case FD_SOFT_ID:
      yaml_get_next_scalar(parser,event);
      /* TODO security check integer */
      sscanf((const char *)event->data.scalar.value,"%d",
	     &filter_tmp->filter.soft_id);
      break;
    case FD_PORT:
      yaml_get_next_scalar(parser,event);
      sscanf((const char *)event->data.scalar.value,"%d",
	     &filter_tmp->port);
      break;

    case FD_INPUT:
    case FD_ACTION:
      return 0;
      break;
    default:
      yaml_error(value,depth);
      break;
    } //end switch case 1
    break; //case1
    

    /**
     * depth 2
     **/
  case 2:
    switch(keyfromstring(value,lkt_2,LKT2_NKEYS)) {
      /* FLOW_TYPE */
    case FD_FLOW_TYPE :
      switch(keyfromstring((char *)yaml_get_next_scalar(parser,event),
			   lkt_fdir_flow_type,LKT_FDIR_FLOW_TYPE_NKEYS)) {
      case RTE_ETH_FLOW_NONFRAG_IPV4_OTHER:
	filter_tmp->filter.input.flow_type = RTE_ETH_FLOW_NONFRAG_IPV4_OTHER;
	break;
      case RTE_ETH_FLOW_IPV4:
	filter_tmp->filter.input.flow_type = RTE_ETH_FLOW_IPV4;
	break;
      default:
      yaml_error(value,depth);
      break;
      }//end switch flow_type
      break;
      /* FLOW*/
    case FD_FLOW : /*return*/
    case FD_FLOW_EXT :
      return 0 ;



      /* RX_QUEUE */
    case FD_ACT_RX_QUEUE :
      /* TODO security check integer */
      filter_tmp->filter.action.rx_queue = 
	atoi((const char *)yaml_get_next_scalar(parser,event));
      break;

      /* BEHAVIOR */
    case FD_ACT_BEHAVIOR :
      switch(keyfromstring((char *)yaml_get_next_scalar(parser,event),
			   lkt_fdir_behavior,LKT_FDIR_BEHAVIOR_NKEYS)) {
      case RTE_ETH_FDIR_ACCEPT:
	filter_tmp->filter.action.behavior = RTE_ETH_FDIR_ACCEPT;
	break;
      case RTE_ETH_FDIR_REJECT :
	filter_tmp->filter.action.behavior = RTE_ETH_FDIR_REJECT;
	break;
      default:
      yaml_error(value,depth);
      break;
      }//end switch behavior
      break; //break case behavior

      /* REPORT_STATUS */
    case FD_ACT_REPORT_STATUS:
      switch(keyfromstring((char *)yaml_get_next_scalar(parser,event),
			   lkt_fdir_status,LKT_FDIR_STATUS_NKEYS)) {
      case RTE_ETH_FDIR_NO_REPORT_STATUS:
	filter_tmp->filter.action.report_status = RTE_ETH_FDIR_NO_REPORT_STATUS;
	break;
      case RTE_ETH_FDIR_REPORT_ID:
	filter_tmp->filter.action.report_status = RTE_ETH_FDIR_REPORT_ID;
	break;
      case RTE_ETH_FDIR_REPORT_ID_FLEX_4:
	filter_tmp->filter.action.report_status = RTE_ETH_FDIR_REPORT_ID_FLEX_4;
	break;
      case RTE_ETH_FDIR_REPORT_FLEX_8:
	filter_tmp->filter.action.report_status = RTE_ETH_FDIR_REPORT_FLEX_8;
	break;
      default:
	yaml_error(value,depth);
	break;
      }//end switch behavior
      break; //break case report_status

      /* FLOW_FLEX_OFF*/
    case FD_ACT_FLEX_OFF :
      /* TODO security check integer */
      filter_tmp->filter.action.flex_off = 
	atoi((const char*)yaml_get_next_scalar(parser,event));
      break;
  
    default:
      yaml_error(value,depth);
      break;
    }
    break;//case2
    
    /**
     * depth 3
     **/
  case 3:
    switch(keyfromstring(value,lkt_3,LKT3_NKEYS)) {
    case FD_VLAN_TCI:
      filter_tmp->filter.input.flow_ext.vlan_tci = 
	atoi((const char*)yaml_get_next_scalar(parser,event));
      break;
      
    case FD_IP4_FLOW:
      return 0;

    case FD_FLEXBYTES: 
    default:
      yaml_error(value,depth);
      break;
    }
    break; //case 3

    /**
     * depth 4
     **/
  case 4:
    switch(keyfromstring(value,lkt_4,LKT4_NKEYS)) {
    case FD_IP4_SRC_IP:
      inet_aton((const char *)yaml_get_next_scalar(parser,event),
		inp);
      filter_tmp->filter.input.flow.ip4_flow.src_ip = inp->s_addr;

	//atoi((const char *)yaml_get_next_scalar(parser,event));
      break;
    case FD_IP4_DST_IP: 
      inet_aton((const char *)yaml_get_next_scalar(parser,event),
		inp);
      filter_tmp->filter.input.flow.ip4_flow.dst_ip = inp->s_addr;
      //	atoi((const char *)yaml_get_next_scalar(parser,event));
      break;

    default:
      yaml_error(value,depth);
      break;
    }
    break; //case 4

  /**
   * Default
   **/
  default :
    /* Default */
    fprintf(stderr,"Strange depth.. cannot parse it\n");
    break;
  }//endswitch

  free(inp);
  return 0;

}


/**********************************/
struct fdir_parsing* 
yaml_parser(char* file_name, struct fdir_parsing *filter_tmp,
	    int (*callback_func)(struct fdir_parsing*)){


  FILE *fh = fopen(file_name, "r");
  yaml_parser_t parser;
  yaml_event_t  event;   /* New variable */
  //  yaml_token_t token;
  int depth = 0;
  int retval;

  /* Initialize parser */
  if(!yaml_parser_initialize(&parser)){
    fputs("Failed to initialize parser!\n", stderr);
  }
  if(fh == NULL){
    fputs("Failed to open file!\n", stderr);
  }
  
  /* Set input file */
  yaml_parser_set_input_file(&parser, fh);

  /* CODE HERE */
  do {
    if (!yaml_parser_parse(&parser, &event)) {
      printf("Parser error %d\n", parser.error);
      exit(EXIT_FAILURE);
    }
    
    switch(event.type) {
      /**
       * Global EVENTS  (not very useful here)
       **/
    case YAML_NO_EVENT: /* puts("No event!"); */break;
      /* Stream start/end */
    case YAML_STREAM_START_EVENT: /*puts("STREAM START");*/ break;
    case YAML_STREAM_END_EVENT:    /*puts("STREAM END");*/   break;
      /* Block delimeters */
    case YAML_DOCUMENT_START_EVENT:/* puts("<b>Start Document</b>"); break;*/
    case YAML_DOCUMENT_END_EVENT:   /*puts("<b>End Document</b>"); */  break;
    case YAML_MAPPING_START_EVENT:/*  puts("<b>Start Mapping</b>");*/  break;
    case YAML_MAPPING_END_EVENT:/*    puts("<b>End Mapping</b>"); */   break;

      /**
       * Increase depth
       **/
    case YAML_SEQUENCE_START_EVENT: 
      depth ++;
      /* puts("<b>Start Sequence</b>"); */
      break;
    case YAML_SEQUENCE_END_EVENT:   
      depth --;
      if (depth == 0){
	/*printf(" New array\n");*/
	callback_func(filter_tmp);
      }
      /*puts("<b>End Sequence</b>");*/
      break;

      
      /**
       * Data 
       **/
    case YAML_ALIAS_EVENT:  /*printf("Got alias (anchor %s)\n",
			      event.data.alias.anchor); */
      break;
    case YAML_SCALAR_EVENT: 
      
      /*printf("Got scalar (value %s)\n", event.data.scalar.value);*/
      retval = yaml_update(depth,filter_tmp,
			   (char *)event.data.scalar.value,&parser,&event);
      if (retval <0 ){
	fprintf(stderr,"Parser error %d\n", parser.error);
	exit(EXIT_FAILURE);
      }
      break;
      
    }
    if(event.type != YAML_STREAM_END_EVENT)
      yaml_event_delete(&event);
  } while(event.type != YAML_STREAM_END_EVENT);
  yaml_event_delete(&event);
  
 
  /* Cleanup */
  yaml_parser_delete(&parser);
  fclose(fh);
  return filter_tmp;
}


#ifdef UNIT_MAIN
static int
toto( __attribute__((unused)) struct fdir_parsing* dummy){
  
//  printf("toto\n");
  return 0;
}

int main(int argc ,char __attribute__((unused)) **argv){
  /* BEGIN */

  if (argc !=2){
    printf("error : ./app file.yaml");
    exit(1);
  }

  printf("file : %s\n",argv[1]);

  struct fdir_parsing filter_tmp;
  memset(&filter_tmp,0,sizeof(struct fdir_parsing));
  
  yaml_parser(argv[1],&filter_tmp,toto);

  return 0;
}
#endif

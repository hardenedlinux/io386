
#include <sys/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "io386.h"

static const char optstr[]
= "i:o:b:ps";

void pusage(const char* name)
{
  fprintf(stderr,
	  "Wrapper tool around outb(2) and its friends.\n"
	  "Usage: %s -i|-o <width> [-b <base for output>] [-p] [-s <count>] port [data]\n"
	  "<width> = b(unsigned char), w(unsigned short), l(unsigned int)\n"
	  "<base> = d(decimal, default), 8(octal), x(for hexadecimal)\n"
	  "serialized output operation reads raw binary stream as an array of <count>\n"
	  "elements of given <width> data from stdin.\n",
	  name);
}

static inline bool strvalid(const char* s)
{
  return (s != NULL) && (s[0] != '\0');
}

static uint8_t get_width(const char* oarg)
{
  if (!strvalid(oarg) || strlen(oarg) > 1)
    return INVALID;

  switch(oarg[0]) {
  case 'b':
    return BYTE;
  case 'w':
    return SHORT;
  case 'l':
    return INT;
  default:
    return INVALID;
  }
}

static uint8_t get_base(const char* oarg)
{
  if (!strvalid(oarg) || strlen(oarg) > 1)
    return INVALID;

  switch(oarg[0]) {
  case 'd':
    return DEC;
  case '8':
    return OCT;
  case 'x':
    return HEX;
  default:
    return INVALID;
  }
}

int main(int argc, char** argv)
{
  if (argc == 1) {
    pusage(argv[0]);
    return EXIT_FAILURE;
  }

  struct op_mode mode = INVALID_OP_MODE;
  bool use_pause = false;
  uint32_t s_count = 0;
  const char* errstr = NULL;
  uint16_t port = 0;
  uint32_t data = 0;
  int ret = 0;

  {
    int opt = 0;
    for(opt = getopt(argc, argv, optstr);
	opt != -1;
	opt = getopt(argc, argv, optstr)) {
      switch(opt) {
      case 'i':
	if (mode.mode == OUTPUT) {
	  errstr = "Input and output are mutually exclusive!\n";
	  goto end_getopt;
	}
	mode.mode = INPUT;
	mode.width = get_width(optarg);
	if (mode.width == INVALID) {
	  errstr = "Invalid width mark!\n";
	  goto end_getopt;
	}
	break;
      case 'o':
	if (mode.mode == INPUT) {
	  errstr = "Input and output are mutually exclusive!\n";
	  goto end_getopt;
	}
	mode.mode = OUTPUT;
	break;
      case 'b':
	mode.base = get_base(optarg);
	if (mode.base == INVALID) {
	  errstr = "Invalid base mark!\n";
	  goto end_getopt;
	}
	break;
      case 'p':
	use_pause = true;
	break;
      case 's':
	{
	  errno = 0;
	  s_count = strtoul(optarg, NULL, 0);
	  if (errno != 0) {
	    errstr = "Invalid count notation!\n";
	    goto end_getopt;
	  }
	}
	break;
      }
    }
  }
  {
    errno = 0;
    uint32_t temp_port = strtoul(argv[optind], NULL, 0);
    if (errno != 0 || temp_port > UINT16_MAX) {
      errstr = "Invalid port!\n";
      goto end_getopt;
    } else {
      port = temp_port;
    }
    if (mode.mode == INPUT) {
      data = strtoul(argv[optind + 1], NULL, 0);
      if (errno != 0) {
	errstr = "Invalid data!\n";
      }
      if ((mode.width = BYTE && data > UINT8_MAX)
	  ||(mode.width = SHORT && data > UINT16_MAX))
	errstr = "Data overflown!\n";
    }
  }
 end_getopt:
  if (errstr) {
    fputs(errstr, stderr);
    pusage(argv[0]);
    return EXIT_FAILURE;
  }

  if(port < 0x3ff) {
    ret = ioperm(port, 1, true);
  } else {
    fprintf(stderr, "Warning: %hu is greater than 0x3ff, calling iopl...\n", port);
    ret = iopl(3);
  }
  if (ret == -1) {
    fprintf(stderr, "Errno = %d: Unable to operate port %hu from userspace!\n",
	    errno,
	    port);
    return EXIT_FAILURE;
  }

  if (s_count == 0) { //IO for a single element
    if (mode.mode == INPUT) {
      switch(mode.width) {
      case BYTE:
	data = (use_pause)?
	  inb_p((uint16_t)port):inb((uint16_t)port);
	printf("%hhu\n", (uint8_t)data);
	break;
      case SHORT:
	data = (use_pause)?
	  inw_p((uint16_t)port):inw((uint16_t)port);
	printf("%hu\n", (uint16_t)data);
	break;
      case INT:
	data = (use_pause)?
	  inl_p((uint16_t)port):inl((uint16_t)port);
	printf("%u\n", (uint32_t)data);
	break;
      default:
	//not supposed to go here.
	return EXIT_FAILURE;
      }
    } else {
      switch(mode.width) {
      case BYTE:
	if (use_pause)
	  outb_p((uint8_t)data, port);
	else
	  outb((uint8_t)data, port);
	break;
      case SHORT:
	if (use_pause)
	  outw_p((uint16_t)data, port);
	else
	  outw((uint16_t)data, port);
	break;
      case INT:
	if (use_pause)
	  outl_p((uint32_t)data, port);
	else
	  outl((uint32_t)data, port);
	break;
      default:
	//not supposed to go here.
	return EXIT_FAILURE;
      }
    }
  } else {//IO for an array of elements
    void* buf = calloc(s_count, sizeof(uint8_t) * (1 << mode.width));
    if (mode.mode == INPUT) {
      switch(mode.width) {
      case BYTE:
	insb(port, buf, s_count);
	break;
      case SHORT:
	insw(port, buf, s_count);
	break;
      case INT:
	insl(port, buf, s_count);
	break;
      default:
	//not supposed to go here.
	ret = EXIT_FAILURE;
	goto s_err;
      }
      fwrite(buf, (1 << mode.width), s_count, stdout);
    } else {
      fread(buf, (1 << mode.width), s_count, stdin);
      switch(mode.width) {
      case BYTE:
	outsb(port, buf, s_count);
	break;
      case SHORT:
	outsw(port, buf, s_count);
	break;
      case INT:
	outsl(port, buf, s_count);
	break;
      default:
	//not supposed to go here.
	return EXIT_FAILURE;
	goto s_err;
      }
    }
  s_err:
    free(buf);
  }
  return ret;
}

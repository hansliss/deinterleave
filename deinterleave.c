#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void usage(char *progname)
{
  fprintf(stderr, "Usage: %s -s <blocksize> -f <file> [-S <slices>]\n", progname);
}

int main(int argc, char *argv[]) {
  FILE *infile = NULL;
  FILE **outfile;
  int slices = 2;
  int ofidx;
  int blocksize = 0;
  int o, n, i;
  unsigned char *inbuf;
  char infilename[1024];
  char fnamebuf[1024];
  while ((o=getopt(argc, argv, "s:f:S:"))!=-1)
    {
      switch (o)
	{
	case 's':
	  blocksize=atoi(optarg);
	  break;
	case 'S':
	  slices = atoi(optarg);
	  break;
	case 'f':
	  strcpy(infilename, optarg);
	  if (!(infile=fopen(infilename, "r"))) {
	    perror(infilename);
	    return -1;
	  }
	  break;
	default:
	  usage(argv[0]);
	  return -1;
	  break;
	}
    }
  if (infile == NULL || blocksize == 0) {
    usage(argv[0]);
    return -1;
  }
  outfile = (FILE **)malloc(slices * sizeof(FILE *));
  for (i = 0; i < slices; i++) {
    sprintf(fnamebuf, "%s_slice%02d", infilename, i);
    if (!(outfile[i]=fopen(fnamebuf,"w"))) {
      perror(fnamebuf);
      return -1;
    }
  }
  inbuf = (unsigned char *)malloc(blocksize);
  ofidx = 0;
  while ((n = fread(inbuf, 1, blocksize, infile)) != 0) {
    if (n < blocksize) {
      fprintf(stderr, "Error: Short read (%d bytes)!\n", n);
    }
    fwrite(inbuf, 1, n, outfile[ofidx]);
    ofidx = (ofidx + 1) % slices;
  }
  fclose(infile);
  for (n=0; n<slices; n++) {
    fclose(outfile[n]);
  }
  free(outfile);
  free(inbuf);
  return 0;
}

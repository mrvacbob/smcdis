/*
 *  buf.cpp
 *  smcdis
 *
 *  Created by Alexander Strange on 12/21/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "buf.h"
#include <stddef.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

static off_t filesize(int fd)
{
	struct stat st;
	
	fstat(fd, &st);
	
	return st.st_size;
}

mapped_file::mapped_file(const char *file)
{
	int fd = open(file, O_RDONLY);
	off_t fs = filesize(fd);
	
	data = mmap(NULL, fs, PROT_READ, MAP_SHARED, fd, 0);
	mmapped = true;
	
	if ((ptrdiff_t)data == -1) {
		data = malloc(fs);
		read(fd, data, fs);
		mmapped = false;
	}
	
	close(fd);
	
	fsize = fs;
}

mapped_file::~mapped_file()
{
	if (mmapped) munmap(data, fsize);
	else free(data);
}
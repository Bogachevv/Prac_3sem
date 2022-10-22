#define BUF_BLOCK_SIZE 64
#define ALLOC_ERR 111

#define RESIZE(buf, buf_len){                                                       \
        buf_len += BUF_BLOCK_SIZE;                                                  \
        buf = realloc(buf, buf_len * sizeof(*buf));                                 \
        if (!(buf)){                                                                \
            fprintf(stderr, "Allocation error: not enough memory\n");               \
            exit(ALLOC_ERR);                                                        \
        }                                                                           \
}

#define PUSH_BACK(buf, buf_len, len, elm){                                          \
	if (len >= (buf_len)){                                                          \
		RESIZE(buf, (buf_len));                                                     \
	}                                                                               \
	(buf)[(len)++] = (elm);                                                         \
}

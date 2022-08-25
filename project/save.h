#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	int save(unsigned char** colors, const char* seed, unsigned int lengthX, unsigned int lengthY, const char *fp, int fp_len);

#ifdef __cplusplus
}
#endif
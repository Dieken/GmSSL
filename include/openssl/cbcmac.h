/* ====================================================================
 * Copyright (c) 2014 - 2016 The GmSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the GmSSL Project.
 *    (http://gmssl.org/)"
 *
 * 4. The name "GmSSL Project" must not be used to endorse or promote
 *    products derived from this software without prior written
 *    permission. For written permission, please contact
 *    guanzhi1980@gmail.com.
 *
 * 5. Products derived from this software may not be called "GmSSL"
 *    nor may "GmSSL" appear in their names without prior written
 *    permission of the GmSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the GmSSL Project
 *    (http://gmssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE GmSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE GmSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */
/*
 * CBC-MAC
 * Attention: CBC-MAC is not secure when message length is not known
 * and un-checked. CMAC is a secure replacement of CBC-MAC. So this
 * module should be only used with legacy applications or protocols.
 * The GM/T standards give IDs for block cipher based MACs. Some of
 * the hardware vendors provide implementation of CBC-MAC with their
 * devices.
 */

#ifndef HEADER_CBCMAC_H
#define HEADER_CBCMAC_H

#include <openssl/evp.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct CBCMAC_CTX_st CBCMAC_CTX;


CBCMAC_CTX *CBCMAC_CTX_new(void);
void CBCMAC_CTX_cleanup(CBCMAC_CTX *ctx);
void CBCMAC_CTX_free(CBCMAC_CTX *ctx);

EVP_CIPHER_CTX *CBCMAC_CTX_get0_cipher_ctx(CBCMAC_CTX *ctx);
int CBCMAC_CTX_copy(CBCMAC_CTX *to, const CBCMAC_CTX *from);

int CBCMAC_Init(CBCMAC_CTX *ctx, const void *key, size_t keylen,
	const EVP_CIPHER *cipher, ENGINE *impl);
int CBCMAC_Update(CBCMAC_CTX *ctx, const void *data, size_t datalen);
int CBCMAC_Final(CBCMAC_CTX *ctx, unsigned char *out, size_t *outlen);
int CBCMAC_resume(CBCMAC_CTX *ctx);

#ifdef  __cplusplus
}
#endif
#endif

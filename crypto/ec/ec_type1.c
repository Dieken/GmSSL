/* ====================================================================
 * Copyright (c) 2016 The GmSSL Project.  All rights reserved.
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
 *
 */

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/bn_gfp2.h>

EC_GROUP *EC_GROUP_generate_type1curve(const BIGNUM *order, BN_CTX *bn_ctx)
{
	ECerr(EC_F_EC_GROUP_GENERATE_TYPE1CURVE, 0);
	return 0;
}

EC_GROUP *EC_GROUP_new_type1curve_ex(const BIGNUM *p, const BIGNUM *a,
	const BIGNUM *b, const unsigned char *point, size_t pointlen,
	const BIGNUM *order, const BIGNUM *cofactor, BN_CTX *bn_ctx)
{
	return NULL;
}

EC_GROUP *EC_GROUP_new_type1curve(const BIGNUM *p,
	const BIGNUM *x, const BIGNUM *y, const BIGNUM *order, BN_CTX *bn_ctx)
{
	int e = 1;
	EC_GROUP *ret = NULL;
	BIGNUM *a = NULL;
	BIGNUM *b = NULL;
	EC_POINT *point = NULL;

	if (!p || !x || !y || !order) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, ERR_R_PASSED_NULL_PARAMETER);
		return NULL;
	}

	/* check p = 11 (mod 12) */
	if (BN_mod_word(p, 12) != 11) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, EC_R_INVALID_TYPE1CURVE);
		return NULL;
	}

	BN_CTX_start(bn_ctx);
	a = BN_CTX_get(bn_ctx);
	b = BN_CTX_get(bn_ctx);

	if (!a || !b) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, ERR_R_MALLOC_FAILURE);
		goto end;
	}

	BN_zero(a);
	BN_one(b);

	if (!(ret = EC_GROUP_new_curve_GFp(p, a, b, bn_ctx))) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, EC_R_INVALID_TYPE1CURVE);
		goto end;
	}

	/* prepare generator point from (x, y) */
	if (!(point = EC_POINT_new(ret))) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, ERR_R_MALLOC_FAILURE);
		goto end;
	}
	if (!EC_POINT_set_affine_coordinates_GFp(ret, point, x, y, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, EC_R_INVALID_TYPE1CURVE);
		goto end;
	}

	/*
	 * calculate cofactor h = (p + 1)/n
	 * check n|(p + 1) where n is the order
	 */
	if (!BN_copy(a, p)) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_add_word(a, 1)) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, ERR_R_BN_LIB);
		goto end;
	}
	/* check (p + 1)%n == 0 */
	if (!BN_div(a, b, a, order, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_is_zero(b)) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, EC_R_INVLID_TYPE1CURVE);
		goto end;
	}

	/* set order and cofactor */
	if (!EC_GROUP_set_generator(ret, point, order, a)) {
		ECerr(EC_F_EC_GROUP_NEW_TYPE1CURVE, EC_R_INVALID_TYPE1CURVE);
		goto end;
	}

	e = 0;

end:
	if (e && ret) {
		EC_GROUP_free(ret);
		ret = NULL;
	}
	BN_CTX_end(bn_ctx);
	EC_POINT_free(point);
	return ret;
}

int EC_GROUP_is_type1curve(const EC_GROUP *group, BN_CTX *bn_ctx)
{
	ECerr(EC_F_EC_GROUP_IS_TYPE1CURVE, 0);
	return 0;
}

/*
 * zeta = F_p((p-1)/2) + ((F_p(3)^((p + 1)/4))/2) * i, in F_p^2
 * which is used in phi() mapping in tate pairing over type1 curve
 */
BN_GFP2 *EC_GROUP_get_type1curve_zeta(const EC_GROUP *group, BN_CTX *bn_ctx)
{
	int e = 1;
	BN_GFP2 *ret = NULL;
	BIGNUM *a = NULL;
	BIGNUM *b = NULL;
	BIGNUM *p = NULL;

	if (!group || !bn_ctx) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_PASSED_NULL_PARAMETER);
		return NULL;
	}

	BN_CTX_start(bn_ctx);

	ret = BN_GFP2_new();
	a = BN_CTX_get(bn_ctx);
	b = BN_CTX_get(bn_ctx);
	p = BN_CTX_get(bn_ctx);

	if (!ret || !a || !b || !p) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_MALLOC_FAILURE);
		goto end;
	}

	/* get curve p, a, b and check it is type1 curve
	 * p is prime at least 512 bits, a == 0 and b == 1
	 */
	if (!EC_GROUP_get_curve_GFp(group, a, b, p, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_EC_LIB);
		goto end;
	}
	if (!BN_is_zero(a) || !BN_is_one(b)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, EC_R_INVALID_TYPE1_CURVE);
		goto end;
	}
	if (BN_num_bits(p) < 512) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, EC_R_INVALID_TYPE1_CURVE);
		goto end;
	}

	/*
	 * set ret->a0 = (p - 1)/2
	 */
	if (!BN_copy(ret->a0, p)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_MALLOC_FAILURE);
		goto end;
	}
	if (!BN_sub_word(ret->a0, 1)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_EC_LIB);
		goto end;
	}
	/* BN_div_word() return remainder, while (p - 1)%2 == 0 */
	if (BN_div_word(ret->a0, 2)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, EC_R_INVALID_TYPE1_CURVE);
		goto end;
	}

	/*
	 * ret->a1 = (p + 1)/4, (ret->a1 + 1)%4 == 0
	 */
	if (!BN_copy(ret->a1, p)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_add_word(ret->a1, 1)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}
	if (BN_div_word(ret->a1, 4)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, EC_R_INVALID_TYPE1_CURVE);
		goto end;
	}

	/*
	 * re-use a as 3
	 * ret->a1 = 3^(ret->a1) mod p = 3^((p + 1)/4) mod p
	 */
	if (!BN_set_word(a, 3)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_mod_exp(ret->a1, a, ret->a1, p, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}

	/*
	 * re-use b as 1/2 mod p
	 * ret->a1 = ret->a1 / 2 mod p = (3^((p + 1)/4)) mod p
	 */
	if (!BN_set_word(b, 2)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_mod_inverse(b, b, p, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_mod_mul(ret->a1, ret->a1, b, p, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ZETA, ERR_R_BN_LIB);
		goto end;
	}

	e = 0;
end:
	if (e && ret) {
		BN_GFP2_free(ret);
		ret = NULL;
	}
	BN_CTX_end(bn_ctx);
	return ret;
}

/*
 * eta = (p^2 - 1)/n
 * which is used in the final modular exponentiation of tate pairing over
 * type1 curve
 */
BIGNUM *EC_GROUP_get_type1curve_eta(const EC_GROUP *group, BN_CTX *bn_ctx)
{
	int e = 1;
	BIGNUM *ret = NULL;
	BIGNUM *a = NULL;
	BIGNUM *b = NULL;
	BIGNUM *p = NULL;

	if (!group || !bn_ctx) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_PASSED_NULL_PARAMETER);
		return NULL;
	}

	BN_CTX_start(bn_ctx);

	ret = BN_new();
	a = BN_CTX_get(bn_ctx);
	b = BN_CTX_get(bn_ctx);
	p = BN_CTX_get(bn_ctx);

	if (!ret || !a || !b || !p) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_MALLOC_FAILURE);
		goto end;
	}

	/* get curve p, a, b and check it is type1 curve
	 * p is prime at least 512 bits, a == 0 and b == 1
	 */
	if (!EC_GROUP_get_curve_GFp(group, a, b, p, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_EC_LIB);
		goto end;
	}
	if (!BN_is_zero(a) || !BN_is_one(b)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, EC_R_INVALID_TYPE1_CURVE);
		goto end;
	}
	if (BN_num_bits(p) < 512) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, EC_R_INVALID_TYPE1_CURVE);
		goto end;
	}

	/* get curve order n, re-use a for order n */
	if (!EC_GROUP_get_order(group, a, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_EC_LIB);
		goto end;
	}

	/*
	 * eta = (p^2 - 1)/n,
	 */
	if (!BN_sqr(ret, p, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_sub_word(ret, 1)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_BN_LIB);
		goto end;
	}
	if (!BN_div(ret, NULL, ret, a, bn_ctx)) {
		ECerr(EC_F_EC_GROUP_GET_TYPE1CURVE_ETA, ERR_R_BN_LIB);
		goto end;
	}

	e = 1;
end:
	if (e && ret) {
		BN_free(ret);
		ret = NULL;
	}
	BN_CTX_end(bn_ctx);
	return ret;
}


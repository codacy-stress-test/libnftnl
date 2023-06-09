/*
 * (C) 2013 by Ana Rey Botello <anarey@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <linux/netfilter/nf_tables.h>
#include <libmnl/libmnl.h>
#include <libnftnl/rule.h>
#include <libnftnl/expr.h>

static int test_ok = 1;

static void print_err(const char *test, const char *msg)
{
	test_ok = 0;
	printf("\033[31mERROR:\e[0m [%s] %s\n", test, msg);
}

static void cmp_nftnl_expr_bool(struct nftnl_expr *rule_a,
				struct nftnl_expr *rule_b)
{
	uint32_t maska, maskb;
	uint32_t xora, xorb;

	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_DREG) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_DREG))
		print_err("bool", "Expr BITWISE_DREG mismatches");
	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_SREG) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_SREG))
		print_err("bool", "Expr BITWISE_SREG mismatches");
	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_OP) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_OP))
		print_err("bool", "Expr BITWISE_OP mismatches");
	if (nftnl_expr_get_u16(rule_a, NFTNL_EXPR_BITWISE_LEN) !=
	    nftnl_expr_get_u16(rule_b, NFTNL_EXPR_BITWISE_LEN))
		print_err("bool", "Expr BITWISE_LEN mismatches");
	nftnl_expr_get(rule_a, NFTNL_EXPR_BITWISE_MASK, &maska);
	nftnl_expr_get(rule_b, NFTNL_EXPR_BITWISE_MASK, &maskb);
	if (maska != maskb)
		print_err("bool", "Size of BITWISE_MASK mismatches");
	nftnl_expr_get(rule_a, NFTNL_EXPR_BITWISE_XOR, &xora);
	nftnl_expr_get(rule_b, NFTNL_EXPR_BITWISE_XOR, &xorb);
	if (xora != xorb)
		print_err("bool", "Size of BITWISE_XOR mismatches");
}

static void cmp_nftnl_expr_lshift(struct nftnl_expr *rule_a,
				  struct nftnl_expr *rule_b)
{
	uint32_t data_a, data_b;

	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_DREG) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_DREG))
		print_err("lshift", "Expr BITWISE_DREG mismatches");
	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_SREG) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_SREG))
		print_err("lshift", "Expr BITWISE_SREG mismatches");
	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_OP) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_OP))
		print_err("lshift", "Expr BITWISE_OP mismatches");
	if (nftnl_expr_get_u16(rule_a, NFTNL_EXPR_BITWISE_LEN) !=
	    nftnl_expr_get_u16(rule_b, NFTNL_EXPR_BITWISE_LEN))
		print_err("lshift", "Expr BITWISE_LEN mismatches");
	nftnl_expr_get(rule_a, NFTNL_EXPR_BITWISE_DATA, &data_a);
	nftnl_expr_get(rule_b, NFTNL_EXPR_BITWISE_DATA, &data_b);
	if (data_a != data_b)
		print_err("lshift", "Expr BITWISE_DATA mismatches");
}

static void cmp_nftnl_expr_rshift(struct nftnl_expr *rule_a,
				  struct nftnl_expr *rule_b)
{
	uint32_t data_a, data_b;

	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_DREG) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_DREG))
		print_err("rshift", "Expr BITWISE_DREG mismatches");
	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_SREG) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_SREG))
		print_err("rshift", "Expr BITWISE_SREG mismatches");
	if (nftnl_expr_get_u32(rule_a, NFTNL_EXPR_BITWISE_OP) !=
	    nftnl_expr_get_u32(rule_b, NFTNL_EXPR_BITWISE_OP))
		print_err("rshift", "Expr BITWISE_OP mismatches");
	if (nftnl_expr_get_u16(rule_a, NFTNL_EXPR_BITWISE_LEN) !=
	    nftnl_expr_get_u16(rule_b, NFTNL_EXPR_BITWISE_LEN))
		print_err("rshift", "Expr BITWISE_LEN mismatches");
	nftnl_expr_get(rule_a, NFTNL_EXPR_BITWISE_DATA, &data_a);
	nftnl_expr_get(rule_b, NFTNL_EXPR_BITWISE_DATA, &data_b);
	if (data_a != data_b)
		print_err("rshift", "Expr BITWISE_DATA mismatches");
}

static void test_bool(void)
{
	struct nftnl_rule *a, *b = NULL;
	struct nftnl_expr *ex = NULL;
	struct nlmsghdr *nlh;
	char buf[4096];
	struct nftnl_expr_iter *iter_a, *iter_b = NULL;
	struct nftnl_expr *rule_a, *rule_b = NULL;
	uint32_t mask = 0x01010101;
	uint32_t xor = 0x12345678;

	a = nftnl_rule_alloc();
	b = nftnl_rule_alloc();
	if (a == NULL || b == NULL)
		print_err("bool", "OOM");
	ex = nftnl_expr_alloc("bitwise");
	if (ex == NULL)
		print_err("bool", "OOM");

	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_SREG, 0x12345678);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_DREG, 0x78123456);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_OP, NFT_BITWISE_BOOL);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_LEN, 0x56781234);
	nftnl_expr_set(ex, NFTNL_EXPR_BITWISE_MASK, &mask, sizeof(mask));
	nftnl_expr_set(ex, NFTNL_EXPR_BITWISE_XOR, &xor, sizeof(xor));

	nftnl_rule_add_expr(a, ex);

	nlh = nftnl_nlmsg_build_hdr(buf, NFT_MSG_NEWRULE, AF_INET, 0, 1234);
	nftnl_rule_nlmsg_build_payload(nlh, a);

	if (nftnl_rule_nlmsg_parse(nlh, b) < 0)
		print_err("bool", "parsing problems");

	iter_a = nftnl_expr_iter_create(a);
	iter_b = nftnl_expr_iter_create(b);
	if (iter_a == NULL || iter_b == NULL)
		print_err("bool", "OOM");

	rule_a = nftnl_expr_iter_next(iter_a);
	rule_b = nftnl_expr_iter_next(iter_b);
	if (rule_a == NULL || rule_b == NULL)
		print_err("bool", "OOM");

	if (nftnl_expr_iter_next(iter_a) != NULL ||
	    nftnl_expr_iter_next(iter_b) != NULL)
		print_err("bool", "More 1 expr.");

	nftnl_expr_iter_destroy(iter_a);
	nftnl_expr_iter_destroy(iter_b);

	cmp_nftnl_expr_bool(rule_a,rule_b);

	nftnl_rule_free(a);
	nftnl_rule_free(b);
}

static void test_lshift(void)
{
	struct nftnl_rule *a, *b = NULL;
	struct nftnl_expr *ex = NULL;
	struct nlmsghdr *nlh;
	char buf[4096];
	struct nftnl_expr_iter *iter_a, *iter_b = NULL;
	struct nftnl_expr *rule_a, *rule_b = NULL;

	a = nftnl_rule_alloc();
	b = nftnl_rule_alloc();
	if (a == NULL || b == NULL)
		print_err("lshift", "OOM");
	ex = nftnl_expr_alloc("bitwise");
	if (ex == NULL)
		print_err("lshift", "OOM");

	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_SREG, 0x12345678);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_DREG, 0x78123456);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_OP, NFT_BITWISE_LSHIFT);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_LEN, 0x56781234);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_DATA, 13);

	nftnl_rule_add_expr(a, ex);

	nlh = nftnl_nlmsg_build_hdr(buf, NFT_MSG_NEWRULE, AF_INET, 0, 1234);
	nftnl_rule_nlmsg_build_payload(nlh, a);

	if (nftnl_rule_nlmsg_parse(nlh, b) < 0)
		print_err("lshift", "parsing problems");

	iter_a = nftnl_expr_iter_create(a);
	iter_b = nftnl_expr_iter_create(b);
	if (iter_a == NULL || iter_b == NULL)
		print_err("lshift", "OOM");

	rule_a = nftnl_expr_iter_next(iter_a);
	rule_b = nftnl_expr_iter_next(iter_b);
	if (rule_a == NULL || rule_b == NULL)
		print_err("lshift", "OOM");

	if (nftnl_expr_iter_next(iter_a) != NULL ||
	    nftnl_expr_iter_next(iter_b) != NULL)
		print_err("lshift", "More 1 expr.");

	nftnl_expr_iter_destroy(iter_a);
	nftnl_expr_iter_destroy(iter_b);

	cmp_nftnl_expr_lshift(rule_a,rule_b);

	nftnl_rule_free(a);
	nftnl_rule_free(b);
}

static void test_rshift(void)
{
	struct nftnl_rule *a, *b = NULL;
	struct nftnl_expr *ex = NULL;
	struct nlmsghdr *nlh;
	char buf[4096];
	struct nftnl_expr_iter *iter_a, *iter_b = NULL;
	struct nftnl_expr *rule_a, *rule_b = NULL;

	a = nftnl_rule_alloc();
	b = nftnl_rule_alloc();
	if (a == NULL || b == NULL)
		print_err("rshift", "OOM");
	ex = nftnl_expr_alloc("bitwise");
	if (ex == NULL)
		print_err("rshift", "OOM");

	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_SREG, 0x12345678);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_DREG, 0x78123456);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_OP, NFT_BITWISE_RSHIFT);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_LEN, 0x56781234);
	nftnl_expr_set_u32(ex, NFTNL_EXPR_BITWISE_DATA, 17);

	nftnl_rule_add_expr(a, ex);

	nlh = nftnl_nlmsg_build_hdr(buf, NFT_MSG_NEWRULE, AF_INET, 0, 1234);
	nftnl_rule_nlmsg_build_payload(nlh, a);

	if (nftnl_rule_nlmsg_parse(nlh, b) < 0)
		print_err("rshift", "parsing problems");

	iter_a = nftnl_expr_iter_create(a);
	iter_b = nftnl_expr_iter_create(b);
	if (iter_a == NULL || iter_b == NULL)
		print_err("rshift", "OOM");

	rule_a = nftnl_expr_iter_next(iter_a);
	rule_b = nftnl_expr_iter_next(iter_b);
	if (rule_a == NULL || rule_b == NULL)
		print_err("rshift", "OOM");

	if (nftnl_expr_iter_next(iter_a) != NULL ||
	    nftnl_expr_iter_next(iter_b) != NULL)
		print_err("rshift", "More 1 expr.");

	nftnl_expr_iter_destroy(iter_a);
	nftnl_expr_iter_destroy(iter_b);

	cmp_nftnl_expr_rshift(rule_a,rule_b);

	nftnl_rule_free(a);
	nftnl_rule_free(b);
}

int main(int argc, char *argv[])
{
	test_bool();
	if (!test_ok)
		exit(EXIT_FAILURE);

	test_lshift();
	if (!test_ok)
		exit(EXIT_FAILURE);

	test_rshift();
	if (!test_ok)
		exit(EXIT_FAILURE);

	printf("%s: \033[32mOK\e[0m\n", argv[0]);
	return EXIT_SUCCESS;
}

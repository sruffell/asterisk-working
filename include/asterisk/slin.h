/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2007, Digium, Inc.
 *
 * Russell Bryant <russell@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/* Source: beep.gsm
 * Converted to beep.sln via file convert, then converted to hex:
 * od -An -tx1 beep.sln | awk '{for (i=1; i<NF; i++) printf "0x%s, ", $i} {printf("0x%s,\n", $NF)}'
 * Samples were truncated at 160 and 320 bytes.
 */

static uint8_t ex_slin8[] = {
	0x00, 0x00, 0x60, 0x00, 0x68, 0x00, 0x48, 0x00, 0xc8, 0xff, 0xa8, 0xff, 0xc8, 0xff, 0x40, 0x00,
	0x60, 0x00, 0x40, 0x00, 0x80, 0x00, 0x80, 0x00, 0x58, 0x00, 0x00, 0xff, 0x50, 0xfe, 0xa0, 0xfe,
	0xd8, 0xff, 0xc0, 0x00, 0xe0, 0x00, 0x58, 0x02, 0xa8, 0x02, 0xb0, 0x01, 0xf0, 0xff, 0xf8, 0xfe,
	0x00, 0xff, 0x00, 0xfd, 0x78, 0xfc, 0x00, 0xfe, 0x48, 0x00, 0x38, 0x02, 0x40, 0x03, 0x20, 0x04,
	0x78, 0x03, 0x90, 0x01, 0x00, 0xff, 0x38, 0xfd, 0xe8, 0xfc, 0x18, 0xfc, 0x90, 0xfc, 0x48, 0xfe,
	0xe0, 0x00, 0x50, 0x03, 0x10, 0x05, 0x88, 0x05, 0x58, 0x04, 0x88, 0x01, 0x68, 0xfe, 0xe8, 0xfb,
	0x80, 0xfa, 0x90, 0xfa, 0x30, 0xfc, 0xb0, 0xff, 0x68, 0x03, 0x18, 0x06, 0x20, 0x07, 0x48, 0x06,
	0xb0, 0x03, 0x50, 0xff, 0xc0, 0xfa, 0xd0, 0xf7, 0xd8, 0xf6, 0x88, 0xf8, 0x50, 0xfc, 0x48, 0x01,
	0x48, 0x06, 0x70, 0x09, 0xe0, 0x09, 0x50, 0x07, 0xa8, 0x02, 0xe0, 0xfc, 0x20, 0xf7, 0xf0, 0xf3,
	0x68, 0xf5, 0xf8, 0xf9, 0x10, 0x00, 0x78, 0x06, 0x70, 0x0b, 0x00, 0x0d, 0xd8, 0x0a, 0xa8, 0x05,
} __attribute__ ((aligned (2)));

static uint8_t ex_slin16[] = {
	0x00, 0x00, 0x60, 0x00, 0x68, 0x00, 0x48, 0x00, 0xc8, 0xff, 0xa8, 0xff, 0xc8, 0xff, 0x40, 0x00,
	0x60, 0x00, 0x40, 0x00, 0x80, 0x00, 0x80, 0x00, 0x58, 0x00, 0x00, 0xff, 0x50, 0xfe, 0xa0, 0xfe,
	0xd8, 0xff, 0xc0, 0x00, 0xe0, 0x00, 0x58, 0x02, 0xa8, 0x02, 0xb0, 0x01, 0xf0, 0xff, 0xf8, 0xfe,
	0x00, 0xff, 0x00, 0xfd, 0x78, 0xfc, 0x00, 0xfe, 0x48, 0x00, 0x38, 0x02, 0x40, 0x03, 0x20, 0x04,
	0x78, 0x03, 0x90, 0x01, 0x00, 0xff, 0x38, 0xfd, 0xe8, 0xfc, 0x18, 0xfc, 0x90, 0xfc, 0x48, 0xfe,
	0xe0, 0x00, 0x50, 0x03, 0x10, 0x05, 0x88, 0x05, 0x58, 0x04, 0x88, 0x01, 0x68, 0xfe, 0xe8, 0xfb,
	0x80, 0xfa, 0x90, 0xfa, 0x30, 0xfc, 0xb0, 0xff, 0x68, 0x03, 0x18, 0x06, 0x20, 0x07, 0x48, 0x06,
	0xb0, 0x03, 0x50, 0xff, 0xc0, 0xfa, 0xd0, 0xf7, 0xd8, 0xf6, 0x88, 0xf8, 0x50, 0xfc, 0x48, 0x01,
	0x48, 0x06, 0x70, 0x09, 0xe0, 0x09, 0x50, 0x07, 0xa8, 0x02, 0xe0, 0xfc, 0x20, 0xf7, 0xf0, 0xf3,
	0x68, 0xf5, 0xf8, 0xf9, 0x10, 0x00, 0x78, 0x06, 0x70, 0x0b, 0x00, 0x0d, 0xd8, 0x0a, 0xa8, 0x05,
	0xa8, 0xfe, 0x28, 0xf8, 0x28, 0xf4, 0x90, 0xf3, 0x98, 0xf6, 0x50, 0xfc, 0x78, 0x03, 0x80, 0x09,
	0x98, 0x0c, 0x70, 0x0b, 0xd8, 0x06, 0x48, 0x00, 0xe0, 0xf8, 0x70, 0xf3, 0xb8, 0xf1, 0xc8, 0xf4,
	0xf8, 0xfa, 0x68, 0x02, 0x50, 0x0a, 0x40, 0x0f, 0xa8, 0x0f, 0x98, 0x0b, 0x80, 0x04, 0x50, 0xfc,
	0x88, 0xf4, 0x40, 0xf0, 0xc8, 0xf0, 0x30, 0xf5, 0x78, 0xfc, 0xa8, 0x04, 0x00, 0x0c, 0xa8, 0x0f,
	0x98, 0x0e, 0xa8, 0x08, 0x30, 0x00, 0xc0, 0xf7, 0x80, 0xf1, 0x80, 0xef, 0x58, 0xf2, 0x20, 0xf9,
	0xb0, 0x01, 0x90, 0x09, 0x68, 0x0f, 0xc0, 0x10, 0x20, 0x0d, 0x30, 0x05, 0xd8, 0xfb, 0xf0, 0xf3,
	0x98, 0xef, 0x20, 0xf0, 0x58, 0xf5, 0xb8, 0xfd, 0x90, 0x06, 0x58, 0x0d, 0x58, 0x10, 0x90, 0x0e,
	0x88, 0x08, 0xe8, 0xff, 0x78, 0xf7, 0xb8, 0xf1, 0xa0, 0xef, 0x40, 0xf2, 0xd8, 0xf8, 0x80, 0x02,
	0x60, 0x0b, 0xc0, 0x10, 0xa0, 0x11, 0x78, 0x0d, 0x70, 0x05, 0x30, 0xfb, 0x98, 0xf2, 0x20, 0xee,
	0x28, 0xef, 0x20, 0xf5, 0x48, 0xfe, 0xf8, 0x07, 0x28, 0x0f, 0xd0, 0x11, 0x18, 0x0e, 0x18, 0x06,
} __attribute__ ((aligned (2)));

static inline struct ast_frame *slin8_sample(void)
{
	static struct ast_frame f = {
		.frametype = AST_FRAME_VOICE,
		.subclass.codec = AST_FORMAT_SLINEAR,
		.datalen = sizeof(ex_slin8),
		.samples = ARRAY_LEN(ex_slin8) / 2,
		.mallocd = 0,
		.offset = 0,
		.src = __PRETTY_FUNCTION__,
		.data.ptr = ex_slin8,
	};

	return &f;
}

static inline struct ast_frame *slin16_sample(void)
{
	static struct ast_frame f = {
		.frametype = AST_FRAME_VOICE,
		.subclass.codec = AST_FORMAT_SLINEAR16,
		.datalen = sizeof(ex_slin16),
		.samples = ARRAY_LEN(ex_slin16) / 2,
		.mallocd = 0,
		.offset = 0,
		.src = __PRETTY_FUNCTION__,
		.data.ptr = ex_slin16,
	};

	return &f;
}

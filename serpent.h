/* serpent.h
 *
 * The serpent block cipher.
 */

/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2001 Niels M�ller
 *  
 * The nettle library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 * 
 * The GNU MP Library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

/* Serpent is a 128-bit block cipher that accepts a key size of 256
 * bits, designed by Ross Anderson, Eli Biham, and Lars Knudsen. See
 * http://www.cl.cam.ac.uk/~rja14/serpent.html for details.
 */

#ifndef NETTLE_SERPENT_H_INCLUDED
#define NETTLE_SERPENT_H_INCLUDED

#include <stdint.h>

#define SERPENT_BLOCKSIZE 16

/* Other key lengths are possible, but we only use 256 bits.  Besides, the
   design of Serpent makes other key lengths useless; they cheated with the
   AES requirements, using a 256-bit key length exclusively and just padding
   it out if the desired key length was less, so there really is no advantage
   to using key lengths less than 256 bits. */
#define SERPENT_KEYSIZE 32

/* Allow keys of size 128 <= bits <= 256 */

#define SERPENT_MIN_KEY_SIZE 16
#define SERPENT_MAX_KEY_SIZE 32

struct serpent_ctx
{
  uint32_t keys[33][4];  /* key schedule */
};

void
serpent_set_key(struct serpent_ctx *ctx,
                unsigned length, const uint8_t *key);

void
serpent_encrypt(struct serpent_ctx *ctx,
                unsigned length, uint8_t *dst,
                const uint8_t *src);
void
serpent_decrypt(struct serpent_ctx *ctx,
                unsigned length, uint8_t *dst,
                const uint8_t *src);

#endif /* NETTLE_SERPENT_H_INCLUDED */

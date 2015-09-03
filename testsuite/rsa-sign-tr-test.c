#include "testutils.h"
#include "knuth-lfib.h"
#include "nettle-internal.h"

#define MSG1 ((uint8_t*)"None so blind as those who will not see")
#define MSG2 ((uint8_t*)"Fortune knocks once at every man's door")

static void
test_rsa_sign_tr(struct rsa_public_key *pub,
	     struct rsa_private_key *key,
	     unsigned di_length,
	     const uint8_t *di,
	     mpz_t expected)
{
  mpz_t signature;
  struct knuth_lfib_ctx lfib;

  knuth_lfib_init(&lfib, 1111);

  mpz_init(signature);

  /* Try bad private key */
  mpz_add_ui(key->p, key->p, 2);

  ASSERT(!rsa_pkcs1_sign_tr(pub, key,
		    &lfib, (nettle_random_func *) knuth_lfib_random,
		    di_length, di, signature));

  mpz_sub_ui(key->p, key->p, 2);

  ASSERT(!mpz_cmp_ui(signature, 0));

  /* Try the good private key */
  ASSERT(rsa_pkcs1_sign_tr(pub, key,
		    &lfib, (nettle_random_func *) knuth_lfib_random,
		    di_length, di, signature));

  if (verbose)
    {
      fprintf(stderr, "rsa-pkcs1-tr signature: ");
      mpz_out_str(stderr, 16, signature);
      fprintf(stderr, "\nrsa-pkcs1-tr expected: ");
      mpz_out_str(stderr, 16, expected);
      fprintf(stderr, "\n");
    }

  ASSERT (mpz_cmp(signature, expected) == 0);

  /* Try bad data */
  ASSERT (!rsa_pkcs1_verify(pub, 16, (void*)"The magick words", signature));

  /* Try correct data */
  ASSERT (rsa_pkcs1_verify(pub, di_length, di, signature));

  /* Try bad signature */
  mpz_combit(signature, 17);
  ASSERT (!rsa_pkcs1_verify(pub, di_length, di, signature));

  mpz_clear(signature);
}


void
test_main(void)
{
  struct rsa_public_key pub;
  struct rsa_private_key key;

  mpz_t expected;

  mpz_init(expected);

  rsa_private_key_init(&key);
  rsa_public_key_init(&pub);

  test_rsa_set_key_1(&pub, &key);

  /* Test signatures */
  mpz_set_str(expected,
	      "5a1d95b71a244dec17887b3fa4502439fd9d868ee12f716fbb53b2"
	      "e7ca60e6922bfdb893cd62dba8e16ca7a4374962d407b6137b4322"
	      "c76331588e087c823162349e40bb0cca762c7f253ea46141f88118"
	      "78788b0759e2c880a089ecbdf447ea02ffa504336cb65bd79f20e1"
	      "3d9af6ac5ac49656fa5cc06cae4f9df510",
	      16);

  test_rsa_sign_tr(&pub, &key, sizeof(MSG1)-1, MSG1, expected);

  mpz_set_str(expected,
	      "86114b62c2d22e2c9b075852df18dfb60c2f86ae8c4a852aff5db7"
	      "164984b1ca3752ae0c1174604ceb46bc72870e87687e298210b88b"
	      "48d0c5ac039c94a40349aaea1399aa60d953697916fbe5020c6751"
	      "060e114588a9ef14751326432d564863d4e11e1e73dabd1f628c6e"
	      "8866f8072d2a2dfd10d716d5d07a6ee57", 16);

  test_rsa_sign_tr(&pub, &key, sizeof(MSG2)-1, MSG2, expected);

  /* 777-bit key, generated by
   *
   *   lsh-keygen -a rsa -l 777 -f advanced-hex
   *
   * Interesting because the size of n doesn't equal the sum of the
   * sizes of p and q.
   *
   * (private-key (rsa-pkcs1
   *        (n #013b04440e3eef25 d51c738d508a7fa8 b3445180c342af0f
   *            4cb5a789047300e2 cfc5c5450974cfc2 448aeaaa7f43c374
   *            c9a3b038b181f2d1 0f1a2327fd2c087b a49bf1086969fd2c
   *            d1df3fd69f81fa4b 162cc8bbb363fc95 b7b24b9c53d0c67e
   *            f52b#)
   *        (e #3f1a012d#)
   *        (d #f9bae89dacca6cca c21e0412b4df8355 6fe7c5322bbae8ad
   *            3f11494fd12bc076 d4a7da3050fe109d 2074db09cc6a93b4
   *            745479522558379e a0ddfa74f86c9e9e a22c3b0e93d51447
   *            0feb38105dd35395 63b91ee32776f40c 67b2a175690f7abb
   *            25#)
   *        (p #0b73c990eeda0a2a 2c26416052c85560 0c5c0f5ce86a8326
   *            166acea91786237a 7ff884e66dbfdd3a ab9d9801414c1506
   *            8b#)
   *        (q #1b81c19a62802a41 9c99283331b0badb 08eb0c25ffce0fbf
   *            50017850036f32f3 2132a845b91a5236 61f7b451d587383f
   *            e1#)
   *        (a #0a912fc93a6cca6b 3521725a3065b3be 3c9745e29c93303d
   *            7d29316c6cafa4a2 89945f964fcdea59 1f9d248b0b6734be
   *            c9#)
   *        (b #1658eca933251813 1eb19c77aba13d73 e0b8f4ce986d7615
   *            764c6b0b03c18146 46b7f332c43e05c5 351e09006979ca5b
   *            05#)
   *        (c #0114720dace7b27f 2bf2850c1804869f 79a0aad0ec02e6b4
   *            05e1831619db2f10 bb9b6a8fd5c95df2 eb78f303ea0c0cc8
   *            06#)))
   */

  mpz_set_str(pub.n,
	      "013b04440e3eef25" "d51c738d508a7fa8" "b3445180c342af0f"
	      "4cb5a789047300e2" "cfc5c5450974cfc2" "448aeaaa7f43c374"
	      "c9a3b038b181f2d1" "0f1a2327fd2c087b" "a49bf1086969fd2c"
	      "d1df3fd69f81fa4b" "162cc8bbb363fc95" "b7b24b9c53d0c67e"
	      "f52b", 16);

  mpz_set_str(pub.e, "3f1a012d", 16);

  ASSERT (rsa_public_key_prepare(&pub));

  mpz_set_str(key.p,
	      "0b73c990eeda0a2a" "2c26416052c85560" "0c5c0f5ce86a8326"
	      "166acea91786237a" "7ff884e66dbfdd3a" "ab9d9801414c1506"
	      "8b", 16);

  mpz_set_str(key.q,
	      "1b81c19a62802a41" "9c99283331b0badb" "08eb0c25ffce0fbf"
	      "50017850036f32f3" "2132a845b91a5236" "61f7b451d587383f"
	      "e1", 16);

  mpz_set_str(key.a,
	      "0a912fc93a6cca6b" "3521725a3065b3be" "3c9745e29c93303d"
	      "7d29316c6cafa4a2" "89945f964fcdea59" "1f9d248b0b6734be"
	      "c9", 16);

  mpz_set_str(key.b,
	      "1658eca933251813" "1eb19c77aba13d73" "e0b8f4ce986d7615"
	      "764c6b0b03c18146" "46b7f332c43e05c5" "351e09006979ca5b"
	      "05", 16);

  mpz_set_str(key.c,
	      "0114720dace7b27f" "2bf2850c1804869f" "79a0aad0ec02e6b4"
	      "05e1831619db2f10" "bb9b6a8fd5c95df2" "eb78f303ea0c0cc8"
	      "06", 16);

  ASSERT (rsa_private_key_prepare(&key));
  ASSERT (pub.size == key.size);

  /* Test signatures */
  mpz_set_str(expected,
	      "63629074879115869fbe830c7cd28ac7fb87df06312572428ba412"
	      "f730f1f88e1f26cd60955a7ff81743bb0f8a0ebb4a05ad621544b3"
	      "2d189b436f753de1dcdf9d29ae048585f1408397dcf9381324498e"
	      "c7df9d35f7b221ec1a2a9e426e07ca8b", 16);

  test_rsa_sign_tr(&pub, &key, sizeof(MSG1)-1, MSG1, expected);

  /* Test sha1 signature */
  mpz_set_str(expected,
	      "9f87de78ee3c6df00444b290aef88765c8367a166e3aeb7e4de0fb"
	      "94a38a48452902f6cd5ebefbd1eaa095ca855e565c1d95c4d8ef2d"
	      "3962a4c707001e1e2112028bda4c78d2d303b11b6cae33a777fd4d"
	      "96d766be5e1bff00defc7628a8a7e8a0", 16);

  test_rsa_sign_tr(&pub, &key, sizeof(MSG2)-1, MSG2, expected);

  rsa_private_key_clear(&key);
  rsa_public_key_clear(&pub);
  mpz_clear(expected);
}

#include "GameIn.h"

#define diffie_hellman_l	32
#define diffie_hellman_g	7
#define diffie_hellman_p	23

int main(int argc, char* argv[])
{
	sx_trace_attach(10, "crash report.txt");
	sx_trace();

	char t[64] = {0};
    sx_time_print(t, 64, sx_time_now());
    sx_print(t);

    byte bignum[64] = {0};
    sx_big_number_power(bignum, 7, 48);
    printf("power(7, 48) = ");
    int len = sx_big_number_print(bignum);
    printf("\nnumber lenght: %u\n", len);

    char seckey[diffie_hellman_l + 1] = {0};
    char pubkey[diffie_hellman_l + 1] = {0};
    char finkey[diffie_hellman_l + 1] = {0};

    sx_dh_secret_Key(seckey, diffie_hellman_l);
    sx_dh_public_key(pubkey, seckey, diffie_hellman_l, diffie_hellman_g, diffie_hellman_p );
    sx_dh_final_key(finkey, seckey, pubkey, diffie_hellman_l, diffie_hellman_p);

    printf("sec: %s\n", seckey);
    printf("pub: %s\n", pubkey);
    printf("fin: %s\n", finkey);

    char* txt = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
    int txt_len = sx_str_len(txt);
    char enctxt[128] = { 0 };
    char dectxt[128] = { 0 };

    sx_encrypt(enctxt, txt, txt_len, finkey, diffie_hellman_l);
    sx_decrypt(dectxt, enctxt, txt_len, finkey, diffie_hellman_l);

    printf("basetx: %s\n", txt);
    printf("encode: %s\n", enctxt);
    printf("decode: %s\n", dectxt);

    char base64[128] = {0};
    sx_base64_encode(base64, 128, enctxt, txt_len);
    printf("base64 encode: %s\n", base64);

    char md5[33] = {0};
    sx_md5(md5, dectxt, "hello", null);
    printf("md5: %s\n", md5);


    getchar();
	return 0;
}


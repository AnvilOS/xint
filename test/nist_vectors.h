
struct key_pair
{
    char *d;
    char *Qx;
    char *Qy;
};

extern struct key_pair key_pairs_p_192[];
extern struct key_pair key_pairs_p_224[];
extern struct key_pair key_pairs_p_256[];
extern struct key_pair key_pairs_p_384[];
extern struct key_pair key_pairs_p_521[];

extern struct key_pair key_pairs_k_163[];
extern struct key_pair key_pairs_k_233[];
extern struct key_pair key_pairs_k_283[];
extern struct key_pair key_pairs_k_409[];
extern struct key_pair key_pairs_k_571[];

extern struct key_pair key_pairs_b_163[];
extern struct key_pair key_pairs_b_233[];
extern struct key_pair key_pairs_b_283[];
extern struct key_pair key_pairs_b_409[];
extern struct key_pair key_pairs_b_571[];

struct pkv
{
    char *Qx;
    char *Qy;
    char *result;
};

extern struct pkv pkv_p_192[];
extern struct pkv pkv_p_224[];
extern struct pkv pkv_p_256[];
extern struct pkv pkv_p_384[];
extern struct pkv pkv_p_521[];

extern struct pkv pkv_k_163[];
extern struct pkv pkv_k_233[];
extern struct pkv pkv_k_283[];
extern struct pkv pkv_k_409[];
extern struct pkv pkv_k_571[];

extern struct pkv pkv_b_163[];
extern struct pkv pkv_b_233[];
extern struct pkv pkv_b_283[];
extern struct pkv pkv_b_409[];
extern struct pkv pkv_b_571[];

struct sig_gen
{
    char *msg;
    char *d;
    char *Qx;
    char *Qy;
    char *k;
    char *R;
    char *S;
};

extern struct sig_gen sig_gen_p_224_sha_256[];
extern struct sig_gen sig_gen_p_256_sha_256[];
extern struct sig_gen sig_gen_p_384_sha_256[];
extern struct sig_gen sig_gen_p_521_sha_256[];

extern struct sig_gen sig_gen_k_233_sha_256[];
extern struct sig_gen sig_gen_k_283_sha_256[];
extern struct sig_gen sig_gen_k_409_sha_256[];
extern struct sig_gen sig_gen_k_571_sha_256[];

extern struct sig_gen sig_gen_b_233_sha_256[];
extern struct sig_gen sig_gen_b_283_sha_256[];
extern struct sig_gen sig_gen_b_409_sha_256[];
extern struct sig_gen sig_gen_b_571_sha_256[];

struct sig_ver
{
    char *msg;
    char *Qx;
    char *Qy;
    char *R;
    char *S;
    char *result;
};

extern struct sig_ver sig_ver_p_192_sha_256[];
extern struct sig_ver sig_ver_p_224_sha_256[];
extern struct sig_ver sig_ver_p_256_sha_256[];
extern struct sig_ver sig_ver_p_384_sha_256[];
extern struct sig_ver sig_ver_p_521_sha_256[];

extern struct sig_ver sig_ver_k_163_sha_256[];
extern struct sig_ver sig_ver_k_233_sha_256[];
extern struct sig_ver sig_ver_k_283_sha_256[];
extern struct sig_ver sig_ver_k_409_sha_256[];
extern struct sig_ver sig_ver_k_571_sha_256[];

extern struct sig_ver sig_ver_b_163_sha_256[];
extern struct sig_ver sig_ver_b_233_sha_256[];
extern struct sig_ver sig_ver_b_283_sha_256[];
extern struct sig_ver sig_ver_b_409_sha_256[];
extern struct sig_ver sig_ver_b_571_sha_256[];


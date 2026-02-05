
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
extern struct pkv pkv_k_163[];

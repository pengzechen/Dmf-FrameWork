#include <jansson.h>
#include <jwt.h>
#include <string.h>
/*
void jwt_test_new(char token[])
{
    int ret = 0;
	jwt_t *jwt_parse = NULL;
    char* jwt_token = NULL;
    ret = jwt_new(&jwt_parse);
    time_t iat = time(NULL);

    jwt_set_alg(jwt_parse, JWT_ALG_HS256, "123", 3);

    jwt_add_grant(jwt_parse, "sub", "1234567890");
    jwt_add_grant(jwt_parse, "name", "pzc");
    jwt_add_grant(jwt_parse, "admin", "true");
    jwt_add_grant_int(jwt_parse, "iat", iat);
    jwt_token = jwt_encode_str(jwt_parse);
    
    //jwt_dump_fp(jwt, stderr, 1);
	//fprintf(stderr, "jwt algo %s!\n", jwt_alg_str(JWT_ALG_HS256));
    
    strcpy(token, jwt_token);

    jwt_free(jwt_parse);
    jwt_free_str(jwt_token);
}

void jwt_test_verify(char* token)
{
    int ret;
    jwt_t *jwt_verify = NULL;
    jwt_valid_t *jwt_valid;
    jwt_valid_new(&jwt_valid, JWT_ALG_HS256);
    jwt_valid_set_headers(jwt_valid, 1);
	jwt_valid_set_now(jwt_valid, time(NULL));    
    if( jwt_valid_add_grant(jwt_valid, "sub", "1234567890") == 0 )
    printf("sub ok\n");
    if( jwt_valid_add_grant(jwt_valid, "name", "pzc") == 0 )
    printf("name ok\n");

    ret = jwt_decode(&jwt_verify, token, "123", 3);
	if (ret != 0 || jwt_verify == NULL) {
        return;
		fprintf(stderr, "invalid jwt\n");
	}

    if (jwt_validate(jwt_verify, jwt_valid) != 0) {
		fprintf(stderr, "jwt failed to validate: %08x\n", jwt_valid_get_status(jwt_valid));
		jwt_dump_fp(jwt_verify, stderr, 1);
	}    
}

void jwt_test()
{
    char token[512] = {0};
    jwt_test_new(token);
    printf("%s\n", token);
    jwt_test_verify(token);
    
}
*/
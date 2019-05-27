#ifndef RESPONSE_H
#define RESPONSE_H

/*
 * struct containing the response sent by the Server to the Client with the generated key
 */
struct response_t {
    long long int key;
    char userId[100];
};

#endif //RESPONSE_H
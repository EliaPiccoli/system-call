#ifndef MESSAGE_H
#define MESSAGE_H

/*
 * sizeof of a string
*/
#define STR_SIZE(X) sizeof(char)*strlen(X)

/*
 * struct containing the elements send by the Invia functionality
*/
 struct message_t {
    long mtype;
    char message[2048];
};

#endif //MESSAGE_H

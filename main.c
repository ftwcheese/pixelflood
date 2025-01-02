#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define ADDR "table.c3pixelflut.de"
#define PORT 1337

static void die(char* msg);
static void send_pixel(int sock, int x, int y, int r, int g, int b);
static void read_in_image(char* pathname);
static void append(char*s, char c);
static void send_all_y_pixels(int sock, int y);


int* r;
int* g;
int* b;
int* r_cpy;
int* g_cpy;
int* b_cpy;

int width;
int height;
int append_index;


int main (int argc, char** argv){
    append_index = 0;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        die("socket");
    }

    struct addrinfo hints = {

        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = 0,
        .ai_protocol = 0,
        .ai_canonname = NULL,
        .ai_addr = NULL,
        .ai_next = NULL,
    }; 
    struct addrinfo *addrs = NULL;
    int ret = getaddrinfo("wall.c3pixelflut.de", "1337", &hints, &addrs);

    if(ret !=0){              
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
           
    }

    int flag = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
        perror("setsockopt(REUSEADDR) (non fatal)");
    }

    int status = connect(sock, (struct sockaddr *) addrs->ai_addr, addrs->ai_addrlen);
    if (status < 0) {
        die("\nConnection Failed \n");
    }
    read_in_image("shrek_256.ppm");
    printf("%s","finished reading in picture");
    
    send(sock,"OFFSET 1540 0\n",strlen("OFFSET 1540 0\n"),0);
    
    printf("%s","start sending");
    for (;;) {
        r_cpy = r;
        g_cpy = g;
        b_cpy = b;
        for(int i = 0; i< width;i++){
            send_all_y_pixels(sock,i);
            /*for (int j = 0; j<height;j++){
                send_pixel(sock,j,i,*r_cpy,*g_cpy,*b_cpy);
                r_cpy++;
                g_cpy++;
                b_cpy++;
            }
            */
        }
    }
}

static void send_pixel(int sock, int x, int y, int r, int g, int b){
    char tmp[19] = {0};
    append(tmp,'P');
    append(tmp,'X');
    append(tmp,' ');
    append(tmp,48+x/100);
    append(tmp,48+(x/10)%10);
    append(tmp,48+x%10);
    
    append(tmp,' ');
    append(tmp,48+y/100);
    append(tmp,48+(y/10)%10);
    append(tmp,48+y%10);
    append(tmp,' ');
    append(tmp,((int)(r/16))/10 * 39 + 48 + (r/16));
    
    append(tmp,r%16/10 * 39 + 48 + r%16);
    append(tmp,((int)(g/16))/10 * 39 + 48 + (g/16));
    append(tmp,g%16/10 * 39 + 48 + g%16);
    append(tmp,((int)(b/16))/10 * 39 + 48 + (b/16));
    append(tmp,b%16/10 * 39 + 48 + b%16);
    
    append(tmp,'\n');
    append(tmp,'\0');
    append_index = 0;

    //printf("%.*s",(int)sizeof(tmp), tmp);
    send(sock,tmp,strlen(tmp),0);


}

static void send_all_y_pixels(int sock, int y){
    
    char tmp[(18*256) +1] = {0};

    int r; 
    int g;
    int b;    

    for(int x = 0; x<height; x++){
        r = *r_cpy;
        g = *g_cpy;
        b = *b_cpy;


        append(tmp,'P');
        append(tmp,'X');
        append(tmp,' ');
        append(tmp,48+x/100);
        append(tmp,48+(x/10)%10);
        append(tmp,48+x%10);
    
        append(tmp,' ');
        append(tmp,48+y/100);
        append(tmp,48+(y/10)%10);
        append(tmp,48+y%10);
        append(tmp,' ');
        append(tmp,((int)(r/16))/10 * 39 + 48 + (r/16));
    
        append(tmp,r%16/10 * 39 + 48 + r%16);
        append(tmp,((int)(g/16))/10 * 39 + 48 + (g/16));
        append(tmp,g%16/10 * 39 + 48 + g%16);
        append(tmp,((int)(b/16))/10 * 39 + 48 + (b/16));
        append(tmp,b%16/10 * 39 + 48 + b%16);
    

        append(tmp,'\n');

        r_cpy++;
        g_cpy++;
        b_cpy++;
    }
    append(tmp,'\0');
    append_index = 0;

    //printf("%.*s",(int)sizeof(tmp), tmp);
    send(sock,tmp,strlen(tmp),0);
    
}

static void read_in_image(char* pathname){
    FILE * in;
    in = fopen(pathname, "r");
    width = 0;
    height = 0;
    fscanf(in,"%d %d\n",&width,&height);

    printf(" %d %d", width, height);
    printf(" hier\n");


    int pixels = width*height;
    r = (int*)malloc(4*pixels);
    g = (int*)malloc(4*pixels);
    b = (int*)malloc(4*pixels);

    r_cpy = r;
    g_cpy = g;
    b_cpy = b;

    fscanf(in,"%d}n",r_cpy);
    for(int i = 0; i< pixels; i++){
        fscanf(in,"%d\n",r_cpy);
        fscanf(in,"%d\n",g_cpy);
        fscanf(in,"%d\n",b_cpy);
        r_cpy++;
        g_cpy++;
        b_cpy++;
    }
    r_cpy = r;
    g_cpy = g;
    b_cpy = b;

    for(int i = 0; i <pixels ; i++){
        //printf("%d %d %d %d \n",i,*r_cpy,*g_cpy,*b_cpy);
        r_cpy++;
        g_cpy++;
        b_cpy++;
    }
    fclose(in);
}


static void die(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static void append(char* s, char c){
    
    s[append_index] = c;
    append_index++;
}